<?php

/**
 * @file
 *
 * This script is used to build or update a site using the makefile.
 */

/**
 * @todo:
 * - Accept arguments to override drush make options.
 * - Create build profiles like:
 *   - Development
 *   - Production
 */

define('BUILDROOT', getcwd());
define('UNIQUE', uniqid());

/**
 * Implementation of hook_drush_command().
 */
function drush_build_drush_command() {
  $items['build'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'description' => '(Re)builds a site instance based on a buildfile.',
    'arguments' => array(
      'buildfile' => 'Filename of the buildfile to use for this build.',
    ),
    'examples' => array(
      'drush build example.build' => 'Build the example.build.',
    ),
  );
  return $items;
}

/**
 * Drush callback; make based on the makefile.
 */
function drush_drush_build_build($buildfile = FALSE) {
  if (!$buildfile) {
    $buildfile = drush_build_find_buildfile();
  }

  // Parse .info file reusing some Drush Make functions
  $info = _drush_make_parse_info_file(drush_make_get_data($buildfile));

  $makefile = $info['makefile'];
  $makeoptions = $info['makeoptions'];
  $buildpath = $info['build'];

  drush_shell_exec("rm -rf $buildpath");
  drush_shell_exec("drush make $makefile $buildpath");

  $sitepath = $info['instance'];
  $sitelink = realpath($buildpath) . '/sites/' . $info['site'];

  drush_shell_exec("rm -rf $sitelink");
  drush_shell_exec("ln -s \"$sitepath\" \"$sitelink\"");
}

/**
 * Scan directory looking for buildfiles.
 */
function drush_build_find_buildfile() {
  $files = scandir(getcwd());

  foreach ($files as $file) {
    if (is_file($file) && preg_match('/\.build$/', $file)) {
      $suggestions[] = $file;
    }
  }

  if (count($suggestions) === 1) {
    return current($suggestions);
  }
}
