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
    '#default_value' => variable_get('islandora_gsearcher_gsearch_url', 'localhost:8080/fedoragsearch/rest'),
  );
  $form['islandora_gsearcher_gsearch_user'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('GSearch User'),
    '#description' => t('User to use when communicating with GSearch.'),
    '#default_value' => variable_get('islandora_gsearcher_gsearch_user', 'fedoraAdmin'),
  );
  $form['islandora_gsearcher_gsearch_pass'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('GSearch Password'),
    '#description' => t('Password to use when communicating with GSearch.'),
    '#default_value' => variable_get('islandora_gsearcher_gsearch_pass', 'fedoraAdmin'),
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
  $response = drupal_http_request("http://$user:$password@$url");
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
