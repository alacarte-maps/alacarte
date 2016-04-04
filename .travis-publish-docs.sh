#!/bin/bash

# GH_TOKEN is a github personal OAUTH tokens, create it here:
# https://github.com/settings/tokens/new (only needs “repo“ access.)
# Then, add them as hidden environment variable in the Travis project settings.
GH_REPO="alacarte-maps/alacarte"
WEBROOT="doc/doxygen/html"


skip() {
	echo "SKIPPING: $@" 1>&2
	echo "Exiting..."
	exit 0
}

if [ "${TRAVIS}" = "true" ]; then
	[ "${TRAVIS_PULL_REQUEST}" = "false" ] || skip "Not building docs for pull requests"
	[ "${TRAVIS_BRANCH}" = "master" ] || skip "Only building docs for master branch"
	[ "${TRAVIS_JOB_NUMBER}" = "${TRAVIS_BUILD_NUMBER}.1" ] || skip "Only build docs once"
	git config user.name "Travis CI"
	git config push.default simple
	ghp-import -n -m "Updated documentation from ${TRAVIS_COMMIT}" ${WEBROOT}
	git push -fq "https://${GH_TOKEN}@github.com/${GH_REPO}.git" gh-pages
	echo "documentation was pushed."
fi

# script from https://github.com/alacarte-maps/alacarte/blob/master/.travis-publish-docs.sh
# based on: https://github.com/gadomski/fgt/blob/master/scripts/gh-pages.sh
# TODO: it would be nice to be able to publish the docs of multiple branches in subfolders, e.g. master and stable
