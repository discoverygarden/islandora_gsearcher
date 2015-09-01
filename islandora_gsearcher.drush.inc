<?php
/**
 * @file
 * Drush integration
 */

/**
 * Implements hook_drush_exit().
 *
 * Something during Drush is preventing the usual Drupal exit flow.
 */
function islandora_gsearcher_drush_exit() {
  module_load_include('inc', 'islandora_gsearcher', 'includes/utilities');
  islandora_gsearcher_inform_gsearch(NULL, TRUE);
}
