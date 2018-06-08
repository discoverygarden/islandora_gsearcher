<?php

/**
 * @file
 * Contains \Drupal\islandora_gsearcher\Form\IslandoraGsearcherSettingsForm.
 */

namespace Drupal\islandora_gsearcher\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;

class IslandoraGsearcherSettingsForm extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'islandora_gsearcher_settings_form';
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $config = $this->config('islandora_gsearcher.settings');

    foreach (Element::children($form) as $variable) {
      $config->set($variable, $form_state->getValue($form[$variable]['#parents']));
    }
    $config->save();

    if (method_exists($this, '_submitForm')) {
      $this->_submitForm($form, $form_state);
    }

    parent::submitForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['islandora_gsearcher.settings'];
  }

  public function buildForm(array $form, \Drupal\Core\Form\FormStateInterface $form_state) {
    $form['islandora_gsearcher_gsearch_url'] = [
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => t('GSearch Address'),
      '#description' => t('Endpoint to use when communicating with GSearch.'),
      '#default_value' => \Drupal::config('islandora_gsearcher.settings')->get('islandora_gsearcher_gsearch_url'),
    ];
    $form['islandora_gsearcher_gsearch_user'] = [
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => t('GSearch User'),
      '#description' => t('User to use when communicating with GSearch.'),
      '#default_value' => \Drupal::config('islandora_gsearcher.settings')->get('islandora_gsearcher_gsearch_user'),
    ];
    $form['islandora_gsearcher_gsearch_pass'] = [
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => t('GSearch Password'),
      '#description' => t('Password to use when communicating with GSearch.'),
      '#default_value' => \Drupal::config('islandora_gsearcher.settings')->get('islandora_gsearcher_gsearch_pass'),
    ];
    return parent::buildForm($form, $form_state);
  }

  public function validateForm(array &$form, \Drupal\Core\Form\FormStateInterface $form_state) {
    $user = $form_state->getValue(['islandora_gsearcher_gsearch_user']);
    $password = $form_state->getValue(['islandora_gsearcher_gsearch_pass']);
    $url = $form_state->getValue(['islandora_gsearcher_gsearch_url']);
    // @FIXME
    // drupal_http_request() has been replaced by the Guzzle HTTP client, which is bundled
    // with Drupal core.
    // 
    // 
    // @see https://www.drupal.org/node/1862446
    // @see http://docs.guzzlephp.org/en/latest
    // $response = drupal_http_request("http://$user:$password@$url");

    if ($response->code == 401) {
      $form_state->setErrorByName('', t('Failed to authenticate with GSearch.'));
    }
    elseif ($response->code != 200) {
      $form_state->setErrorByName('', t('GSearch did not return 200. Something may be wrong with your configuration or GSearch.'));
    }
  }

}
?>
