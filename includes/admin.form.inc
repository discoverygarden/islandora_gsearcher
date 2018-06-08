<?php
/**
 * @file
 * Administration functions.
 */

/**
 * Admin settings form.
 */
function islandora_gsearcher_settings_form(array $form, array &$form_state) {
  $form['islandora_gsearcher_gsearch_url'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('GSearch Address'),
    '#description' => t('Endpoint to use when communicating with GSearch.'),
    '#default_value' => \Drupal::config('islandora_gsearcher.settings')->get('islandora_gsearcher_gsearch_url'),
  );
  $form['islandora_gsearcher_gsearch_user'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('GSearch User'),
    '#description' => t('User to use when communicating with GSearch.'),
    '#default_value' => \Drupal::config('islandora_gsearcher.settings')->get('islandora_gsearcher_gsearch_user'),
  );
  $form['islandora_gsearcher_gsearch_pass'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('GSearch Password'),
    '#description' => t('Password to use when communicating with GSearch.'),
    '#default_value' => \Drupal::config('islandora_gsearcher.settings')->get('islandora_gsearcher_gsearch_pass'),
  );
  return system_settings_form($form);
}

/**
 * Admin settings form validation.
 */
function islandora_gsearcher_settings_form_validate(array &$form, array &$form_state) {
  $user = $form_state['values']['islandora_gsearcher_gsearch_user'];
  $password = $form_state['values']['islandora_gsearcher_gsearch_pass'];
  $url = $form_state['values']['islandora_gsearcher_gsearch_url'];
  // @FIXME
// drupal_http_request() has been replaced by the Guzzle HTTP client, which is bundled
// with Drupal core.
// 
// 
// @see https://www.drupal.org/node/1862446
// @see http://docs.guzzlephp.org/en/latest
// $response = drupal_http_request("http://$user:$password@$url");

  if ($response->code == 401) {
    form_set_error('', t('Failed to authenticate with GSearch.'));
  }
  elseif ($response->code != 200) {
    form_set_error(
      '',
      t('GSearch did not return 200. Something may be wrong with your configuration or GSearch.')
    );
  }
}
