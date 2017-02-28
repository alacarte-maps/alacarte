#!/usr/bin/env bash
# Usage cppcheck.sh "<root proj dir>" "<build dir>"

# get absolute paths
proj_dir=`cd "$1"; pwd`
build_dir=`cd "$2"; pwd`

echo "cppcheck:"
echo " version      : `cppcheck --version`"
echo " root proj dir: $proj_dir"
echo " build dir    : $build_dir"

if [ ! -d "$build_dir" ]; then
	echo "'$build_dir' does not exist."
	echo "Run cmake before calling this script!"
	exit 2
fi

# TODO: enable all checks
#cppc_checks="--enable=all"
cppc_checks="--enable=warning,performance,portability,information"
# TODO: do not exclude tests
cppc_exclude_dirs="-i \"$build_dir\" -i \"$proj_dir/extras\" -i \"$proj_dir/tests\""
cppc_include_dirs="-I \"$proj_dir/include/\" -I \"$build_dir/include/\""
# NOTE: suppress 'extras' explicitly since exclude_dirs does not apply to included header files
cppc_suppress="--suppress=missingIncludeSystem --suppress=*:*/extras/*"
# NOTE: quiet: hide files with no errors; verbose: more info if there is a error
cppc_other_flags="--quiet --verbose --error-exitcode=2"
cppc_template="--template=[{file}:{line}]:\n\tSeverity:\t{severity}\n\tId:\t\t{id}\n\tMessage:\t{message}\n"
cppc_arguments="$cppc_checks $cppc_exclude_dirs $cppc_include_dirs $cppc_suppress $cppc_other_flags "$cppc_template""

#echo "cppcheck $cppc_arguments \"$proj_dir\""
cppcheck $cppc_arguments "$proj_dir/src" "$proj_dir/tools" "$proj_dir/tests"
