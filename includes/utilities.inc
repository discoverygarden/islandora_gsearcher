<?php

/**
 * @file
 * Misc helpers.
 */

use Drupal\Core\Url;

/**
 * Inform GSearch about changes to or the creation of an object.
 *
 * @param string $pid
 *   The PID to inform GSearch of.
 * @param bool $send
 *   Whether or not to send information.
 * @param bool $purge
 *   Whether or not to purge the PID from the index.
 */
function islandora_gsearcher_inform_gsearch($pid = NULL, $send = FALSE, $purge = FALSE) {
  static $update_pids = [];
  static $purge_pids = [];
  static $registered = FALSE;

  // Register as shutdown if necessary.
  if (!$registered) {
    drupal_register_shutdown_function('islandora_gsearcher_inform_gsearch', NULL, TRUE);
    $registered = TRUE;
  }

  // Handle PIDs, most recent change is the only one we need to report.
  if ($pid && !$purge && !in_array($pid, $update_pids)) {
    $update_pids[] = $pid;
    if (in_array($pid, $purge_pids)) {
      unset($purge_pids[array_search($pid, $purge_pids)]);
    }
  }
  elseif ($pid && $purge && !in_array($pid, $purge_pids)) {
    $purge_pids[] = $pid;
    if (in_array($pid, $update_pids)) {
      unset($update_pids[array_search($pid, $update_pids)]);
    }
  }

  if ($send) {
    foreach ($update_pids as $pid_to_send) {
      islandora_gsearcher_speak_to_gsearch($pid_to_send, 'index');
    }
    foreach ($purge_pids as $pid_to_send) {
      islandora_gsearcher_speak_to_gsearch($pid_to_send, 'purge');
    }
    // Make sure we don't talk to GSearch more than necessary.
    $update_pids = [];
    $purge_pids = [];
  }
}

/**
 * Sends a request to GSearch.
 *
 * @param string $pid
 *   The PID to send.
 * @param string $opp
 *   The operation to do.
 */
function islandora_gsearcher_speak_to_gsearch($pid, $opp) {
  $config = \Drupal::config('islandora_gsearcher.settings');
  if ($opp == 'index') {
    $action = 'fromPid';
    $verb = 'index';
  }
  elseif ($opp == 'purge') {
    $action = 'deletePid';
    $verb = 'purge';
  }

  $password = $config->get('islandora_gsearcher_gsearch_pass');
  $user = $config->get('islandora_gsearcher_gsearch_user');
  $url = $config->get('islandora_gsearcher_gsearch_url');

  $client = \Drupal::httpClient();
  $url_params = [
    'query' => [
      'operation' => 'updateIndex',
      'action' => $action,
      'value' => $pid,
    ],
  ];
  $url = Url::fromUri("http://$user:$password@$url", $url_params);
  $response = $client->request('GET', $url->toString());

  // Ping the logs in case of failure, GSearch will know more.
  if ($response->code != 200) {
    \Drupal::logger('Islandora GSearcher')->error('GSearch did not return 200 when asked to @verb @pid.', ['@verb' => $verb, '@pid' => $pid]);
  }
}
