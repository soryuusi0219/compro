FROM ubuntu:24.04
COPY tools/setup_verify_env.sh /tmp/setup_verify_env.sh
COPY requirements-verify.txt /tmp/requirements-verify.txt
RUN sh /tmp/setup_verify_env.sh /tmp/requirements-verify.txt && rm -rf /var/lib/apt/lists/*
ENV PATH="/opt/verify/bin:${PATH}"
WORKDIR /repo
