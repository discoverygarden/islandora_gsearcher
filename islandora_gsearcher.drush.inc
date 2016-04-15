<?php
/**
 * @file
 * Drush integration
 */

require_once dirname(__FILE__) . '/includes/utilities.inc';

/**
 * Implements hook_drush_exit().
 *
 * Something during Drush is preventing the usual Drupal exit flow.
 */
function islandora_gsearcher_drush_exit() {
  islandora_gsearcher_inform_gsearch(NULL, TRUE);
}
