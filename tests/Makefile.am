SUBDIRS = dummyatk apps data pyatspi

#TESTS=testrunner.py
TESTS_ENVIRONMENT = PYTHONPATH=$(abs_top_srcdir)/python					\
		    ATSPI_INTROSPECTION_PATH=$(top_srcdir)/xml/introspection		\
		    TEST_DATA_DIRECTORY=$(abs_top_srcdir)/tests/data			\
		    TEST_ATSPI_LIBRARY=$(abs_top_srcdir)/atk-adaptor/.libs/libspiatk.so	\
		    TEST_MODULES_DIRECTORY=$(abs_top_srcdir)/tests/apps/.libs		\
		    TEST_APPLICATION=$(abs_top_srcdir)/tests/apps/test-application

CLEANFILES = *.pyc
