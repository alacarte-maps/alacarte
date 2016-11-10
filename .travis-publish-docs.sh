#!/bin/bash

# GH_TOKEN is a github personal OAUTH tokens, create it here:
# https://github.com/settings/tokens/new (only needs “repo“ access.)
# Then, add them as hidden environment variable in the Travis project settings.
GH_REPO="alacarte-maps/alacarte"
INPUT_BUILD_DIR="build"
INPUT_DOXYGEN_DIR="${INPUT_BUILD_DIR}/doxygen/html"
INPUT_MANPAGE_DIR="${INPUT_BUILD_DIR}/manpages"
OUTPUT_DIR="webroot"
OUTPUT_DOC_DIR="${OUTPUT_DIR}/documentation"
OUTPUT_MANPAGE_DIR="${OUTPUT_DIR}/manpages"

skip() {
	echo "SKIPPING: $@" 1>&2
	echo "Exiting..."
	exit 0
}

if [ "${TRAVIS}" = "true" ]; then
	[ "${TRAVIS_PULL_REQUEST}" = "false" ] || skip "Not building docs for pull requests"
	[ "${TRAVIS_BRANCH}" = "master" ] || skip "Only building docs for master branch"
	[ "${TRAVIS_JOB_NUMBER}" = "${TRAVIS_BUILD_NUMBER}.1" ] || skip "Only build docs once"
	mkdir -p "${OUTPUT_DIR}"
	mv "${INPUT_DOXYGEN_DIR}" "${OUTPUT_DOC_DIR}"
	mkdir -p "${OUTPUT_MANPAGE_DIR}"
	mv "${INPUT_MANPAGE_DIR}/alacarte-maps-server.1.html" "${OUTPUT_MANPAGE_DIR}"
	mv "${INPUT_MANPAGE_DIR}/alacarte-maps-importer.1.html" "${OUTPUT_MANPAGE_DIR}"
	mv "${INPUT_MANPAGE_DIR}/docbook-xsl.css" "${OUTPUT_MANPAGE_DIR}"
	git config user.name "Travis CI"
	git config push.default simple
	ghp-import -n -m "Updated documentation from ${TRAVIS_COMMIT}" "${OUTPUT_DIR}"
	git push -fq "https://${GH_TOKEN}@github.com/${GH_REPO}.git" gh-pages
	echo "documentation was pushed."
fi

# script from: https://github.com/alacarte-maps/alacarte/blob/master/.travis-publish-docs.sh
# based on: https://github.com/gadomski/fgt/blob/master/scripts/gh-pages.sh
# TODO: it would be nice to be able to publish the docs of multiple branches in subfolders, e.g. master and stable
