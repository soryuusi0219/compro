"""Regression tests for header-relative dependencies and source expansion."""

from pathlib import Path
from tempfile import TemporaryDirectory
import unittest
from unittest.mock import patch

import library_manager as manager
from expand_include import Expander


class RelativeIncludeTest(unittest.TestCase):
    def setUp(self):
        self.directory = TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name).resolve()
        for name, value in (
            ("ROOT", self.root),
            ("LIBRARY_DIR", self.root / "library"),
            ("ATCODER_DIR", self.root / "third_party/ac-library/atcoder"),
        ):
            patcher = patch.object(manager, name, value)
            patcher.start()
            self.addCleanup(patcher.stop)

    def write(self, name, body):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        if name.startswith("library/"):
            body = f"// {manager.GITHUB_LIBRARY_BASE}{name}\n#pragma once\n" + body
        path.write_text(body)
        return path

    def test_resolves_siblings_parents_and_source_relative_includes(self):
        self.write("library/math/a.hpp", 'constexpr int value = 42;\n')
        header = self.write("library/math/detail/b.hpp", '#include "../a.hpp"\n')
        other = self.write("library/data/c.hpp", '#include "../math/detail/b.hpp"\n')
        source = self.write("verify/main.cpp", '#include "../library/data/c.hpp"\n')
        self.assertEqual(manager.dependency_includes(header), ["library/math/a.hpp"])
        self.assertEqual(manager.dependency_includes(other), ["library/math/detail/b.hpp"])
        self.assertEqual(manager.dependency_includes(source), ["library/data/c.hpp"])
        manager.validate_graph(manager.dependency_graph())

    def test_expansion_deduplicates_mixed_include_paths(self):
        self.write("library/math/a.hpp", 'constexpr int value = 42;\n')
        self.write("library/math/b.hpp", '#include "./a.hpp"\n')
        source = self.write("verify/main.cpp", '#include "library/math/a.hpp"\n'
                            '#include "../library/math/b.hpp"\n#include <vector>\n')
        expanded = manager.expand_source("verify/main.cpp", manager.dependency_graph(), header=False)
        self.assertEqual(expanded.count("constexpr int value = 42;"), 1)
        self.assertIn("#include <vector>", expanded)
        self.assertNotIn('#include "', expanded)
        expander = Expander(self.root, release=False, comments=False, line_directive=False,
                            expand_acl=False, strip_included_comments=False)
        submitted = "\n".join(expander.expand_file(source, [], is_root=True))
        self.assertEqual(submitted.count("constexpr int value = 42;"), 1)
        self.assertNotIn('#include "', submitted)

    def test_missing_relative_dependency_is_reported(self):
        self.write("library/math/a.hpp", '#include "detail/missing.hpp"\n')
        with self.assertRaisesRegex(manager.LibraryError, "missing dependency library/math/detail/missing.hpp"):
            manager.validate_graph(manager.dependency_graph())

    def test_relative_cycle_is_reported(self):
        self.write("library/math/a.hpp", '#include "detail/b.hpp"\n')
        self.write("library/math/detail/b.hpp", '#include "../a.hpp"\n')
        with self.assertRaisesRegex(manager.LibraryError, "dependency cycle"):
            manager.validate_graph(manager.dependency_graph())

    def test_root_based_library_include_is_rejected_inside_header(self):
        self.write("library/math/a.hpp", "")
        self.write("library/math/b.hpp", '#include "library/math/a.hpp"\n')
        with self.assertRaisesRegex(manager.LibraryError, "use header-relative includes"):
            manager.validate_graph(manager.dependency_graph())

    def test_atcoder_dependencies_keep_their_names(self):
        self.write("third_party/ac-library/atcoder/modint", '#include "atcoder/modint.hpp"\n')
        self.write("third_party/ac-library/atcoder/modint.hpp", "")
        source = self.write("verify/main.cpp", '#include <atcoder/modint>\n')
        graph = manager.dependency_graph()
        manager.validate_graph(graph)
        self.assertEqual(manager.dependency_includes(source), ["atcoder/modint"])
        expanded = manager.expand_source("verify/main.cpp", graph, header=False)
        self.assertIn("#include <atcoder/modint>", expanded)


if __name__ == "__main__":
    unittest.main()
