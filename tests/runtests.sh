export PYTHONPATH=../python
export ATSPI_INTROSPECTION_PATH=../xml/introspection

export testdata=../tests/data
export atspilib=../atk-adaptor/.libs/libspiatk.so
export testmodules=../tests/apps/.libs
export testapp=../tests/apps/test-application

./testrunner.py
