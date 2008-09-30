export PYTHONPATH=$top_srcdir

export TEST_DATA_DIRECTORY=$top_srcdir/tests/data
export TEST_ATSPI_LIBRARY=$top_builddir/atk-adaptor/.libs/libspiatk.so
export TEST_MODULES_DIRECTORY=$top_builddir/tests/apps/.libs
export TEST_APPLICATION=$top_builddir/tests/apps/test-application

$top_srcdir/tests/pyatspi/testrunner -l libaccessibleapp.so -m accessibletest -n AccessibleTest
$top_srcdir/tests/pyatspi/testrunner -l libactionapp.so -m actiontest -n ActionTest
$top_srcdir/tests/pyatspi/testrunner -l libcomponentapp.so -m componenttest -n ComponentTest
$top_srcdir/tests/pyatspi/testrunner -l librelationapp.so -m relationtest -n RelationTest
$top_srcdir/tests/pyatspi/testrunner -l libaccessibleapp.so -m statetest -n StateTest
