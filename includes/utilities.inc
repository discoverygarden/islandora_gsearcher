<?php
/**
 * @file
 * Misc helpers.
 */

/**
 * Inform GSearch about changes to or the creation of an object.
 *
 * @param string $pid
 *   The PID to inform gsearch of.
 * @param bool $send
 *   Whether or not to send information
 */
function islandora_gsearcher_inform_gsearch($pid = NULL, $send = FALSE) {
  static $pids = array();
  static $registered = FALSE;

  // Register as shutdown if necessary.
  if (!$registered) {
    $registered_funcs = drupal_register_shutdown_function('islandora_gsearcher_inform_gsearch', NULL, TRUE);
    $registered = TRUE;
  }

  // Handle PIDs.
  if ($pid && !in_array($pid, $pids)) {
    $pids[] = $pid;
  }
  if ($send) {
    foreach ($pids as $pid_to_send) {
      $password = variable_get('islandora_gsearcher_gsearch_pass', 'fedoraAdmin');
      $user = variable_get('islandora_gsearcher_gsearch_user', 'fedoraAdmin');
      $url = variable_get('islandora_gsearcher_gsearch_url', 'http://localhost:8080/fedoragsearch/rest');
      $response = drupal_http_request(
        url(
          "http://$user:$password@$url",
          array(
            'query' => array(
              'operation' => 'updateIndex',
              'action' => 'fromPid',
              'value' => $pid_to_send,
            ),
          )
        )
      );
    }
    // Make sure we don't talk to GSearch more than necessary.
    $pids = array();
  }
}
