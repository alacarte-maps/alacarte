#!/bin/bash

# GH_TOKEN is a github personal OAUTH tokens, create it here:
# https://github.com/settings/tokens/new (only needs “repo“ access.)
# Then, add them as hidden environment variable in the Travis project settings.
GH_REPO="alacarte-maps/alacarte"

if [ "${TRAVIS}" = "true" ]; then
	[ "${TRAVIS_PULL_REQUEST}" = "false" ] || skip "Not building docs for pull requests"
	[ "${TRAVIS_BRANCH}" = "master" ] || skip "Only building docs for master branch"
	[ "${TRAVIS_JOB_NUMBER}" = "${TRAVIS_BUILD_NUMBER}.1" ] || skip "Only build docs once"
	git config user.name "Travis CI"
	git config push.default simple
	ghp-import -n -m "Updated documentation from ${TRAVIS_COMMIT}" doc/doxygen/html
	git push -fq "https://${GH_TOKEN}@github.com/${GH_REPO}.git" gh-pages
	echo "documentation was pushed."
fi

# script from https://github.com/alacarte-maps/alacarte/blob/master/.travis-publish-docs.sh
# TODO: it would be nice to be able to publish the docs of multiple branches in subfolders, e.g. master and stable
