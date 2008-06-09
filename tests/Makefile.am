SUBDIRS = dummyatk apps clients
EXTRA_DIST = testrunner.py runtests.sh

TESTS=testrunner.py
TESTS_ENVIRONMENT = PYTHONPATH=$(abs_top_srcdir)/python				\
		    ATSPI_INTROSPECTION_PATH=$(top_srcdir)/xml/introspection	\
		    testdata=$(abs_top_srcdir)/tests/data			\
		    atspilib=$(abs_top_srcdir)/atk-adaptor/.libs/libspiatk.so	\
		    testmodules=$(abs_top_srcdir)/tests/apps/.libs		\
		    testapp=$(abs_top_srcdir)/tests/apps/test-application

CLEANFILES = *.pyc
