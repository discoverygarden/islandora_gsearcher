<?php

/**
 * @file
 * Misc helpers.
 */

use Drupal\Core\Url;
use GuzzleHttp\Exception\GuzzleException;
use Psr\Http\Message\ResponseInterface;

use Drupal\islandora_gsearcher\Form\Admin as AdminForm;
use Drupal\islandora_solr\Form\AdminIndexSettings;

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

    islandora_gsearcher_commit_solr();
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
  if ($opp == 'index') {
    $action = 'fromPid';
    $verb = 'index';
  }
  elseif ($opp == 'purge') {
    $action = 'deletePid';
    $verb = 'purge';
  }

  $password = AdminForm::stateGet('islandora_gsearcher_gsearch_pass');
  $user = AdminForm::stateGet('islandora_gsearcher_gsearch_user');
  $url = AdminForm::stateGet('islandora_gsearcher_gsearch_url');

  $client = \Drupal::httpClient();
  $url_params = [
    'query' => [
      'operation' => 'updateIndex',
      'action' => $action,
      'value' => $pid,
    ],
  ];
  $url = Url::fromUri("http://$user:$password@$url", $url_params);
  try {
    $response = $client->request('GET', $url->toString());
  }
  catch (GuzzleException $exception) {
    if ($exception->hasResponse()) {
      $response = $exception->getResponse();
    }
    else {
      \Drupal::logger('Islandora GSearcher')->error('Encountered exception without a response.');
    }
  }
  if (isset($response)) {
    _islandora_gsearcher_check_response($response, 'GSearch did not return 200 when asked to @verb @pid.', ['@verb' => $verb, '@pid' => $pid]);
  }
}

/**
 * Have Solr make the indexed documents available to query.
 */
function islandora_gsearcher_commit_solr() {
  module_load_include('inc', 'islandora_solr', 'includes/luke');
  $url = islandora_solr_check_http(AdminIndexSettings::stateGet('islandora_solr_url'));
  $client = \Drupal::httpClient();
  $url_params = [
    'query' => [
      'stream.body' => '<commit softCommit="true"/>',
    ],
  ];
  $url = Url::fromUri("$url/update", $url_params);
  try {
    $response = $client->request('GET', $url->toString());
  }
  catch (GuzzleException $exception) {
    if ($exception->hasResponse()) {
      $response = $exception->getResponse();
    }
    else {
      \Drupal::logger('Islandora GSearcher')->error('Encountered exception without a response.');
    }
  }
  if (isset($response)) {
    _islandora_gsearcher_check_response($response, 'Solr did not return 200 when asked to commit.');
  }
}

/**
 * Helper; check the response and dump our message accordingly.
 *
 * @param Psr\Http\Message\ResponseInterface $response
 *   The response to check.
 * @param string $message
 *   A message to output on error.
 * @param array $subs
 *   An optional array mapping placeholders onto values to substitute into the
 *   message.
 */
function _islandora_gsearcher_check_response(ResponseInterface $response, $message, array $subs = []) {
  if ($response->getStatusCode() != 200) {
    // Ping the logs in case of failure, GSearch will know more.
    \Drupal::logger('Islandora GSearcher')->error($message, $subs);
  }
}
