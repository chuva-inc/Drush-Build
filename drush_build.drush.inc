<?php

/**
 * @file
 *
 * This script is used to build or update a site using the makefile.
 */

/**
 * @todo:
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
    $buildfile = drush_build_find_buildfile(getcwd());
  }

  // Parse .info file reusing some Drush Make functions
  $info = _drush_make_parse_info_file(drush_make_get_data($buildfile));

  $makefile = $info['makefile'];
  $makeoptions = drush_build_format_makeoptions($info['makeoptions']);
  $buildpath = $info['build'];
  $temppath = $buildpath . '-' . UNIQUE;

  drush_shell_exec_interactive("drush make $makeoptions $makefile $temppath");

  drush_shell_exec("rm -rf $buildpath");
  drush_shell_exec("mv $temppath $buildpath");

  $sitepath = realpath($info['instance']);
  $sitelink = realpath($buildpath) . '/sites/' . $info['site'];

  drush_shell_exec("rm -rf \"$sitelink\"");
  drush_shell_exec("ln -s \"$sitepath\" \"$sitelink\"");
}

/**
 * Format options to a command-line format.
 */
function drush_build_format_makeoptions($options) {
  foreach ($options as $option => $value) {
    if (is_array($value)) {
      $value = implode(',', $value);
    }
    if ($value) {
      $value = '=' . $value;
    }
    $formatted_options[] = '--' . $option . $value;
  }

  return implode(' ', $formatted_options);
}

/**
 * Scan directory looking for buildfiles.
 */
function drush_build_find_buildfile($directory) {
  $files = scandir($directory);

  foreach ($files as $file) {
    if (is_file($file) && preg_match('/\.build$/', $file)) {
      $suggestions[] = $file;
    }
  }

  if (count($suggestions) === 1) {
    return current($suggestions);
  }
}
