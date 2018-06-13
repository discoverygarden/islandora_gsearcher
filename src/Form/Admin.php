<?php

namespace Drupal\islandora_gsearcher\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use GuzzleHttp\ClientInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use GuzzleHttp\Exception\RequestException;

/**
 * Module settings form.
 */
class Admin extends ConfigFormBase {

  protected $httpClient;

  /**
   * Class constructor.
   */
  public function __construct(ClientInterface $http_client) {
    $this->httpClient = $http_client;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    // Instantiates this form class.
    return new static(
    // Load the service required to construct this class.
      $container->get('http_client')
    );
  }

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

    $config->set('islandora_gsearcher_gsearch_url', $form_state->getValue('islandora_gsearcher_gsearch_url'));
    $config->set('islandora_gsearcher_gsearch_user', $form_state->getValue('islandora_gsearcher_gsearch_user'));
    $config->set('islandora_gsearcher_gsearch_pass', $form_state->getValue('islandora_gsearcher_gsearch_pass'));

    $config->save();
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['islandora_gsearcher.settings'];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = $this->config('islandora_gsearcher.settings');
    $form['islandora_gsearcher_gsearch_url'] = [
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => $this->t('GSearch Address'),
      '#description' => $this->t('Endpoint to use when communicating with GSearch.'),
      '#default_value' => $config->get('islandora_gsearcher_gsearch_url'),
    ];
    $form['islandora_gsearcher_gsearch_user'] = [
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => $this->t('GSearch User'),
      '#description' => $this->t('User to use when communicating with GSearch.'),
      '#default_value' => $config->get('islandora_gsearcher_gsearch_user'),
    ];
    $form['islandora_gsearcher_gsearch_pass'] = [
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => $this->t('GSearch Password'),
      '#description' => $this->t('Password to use when communicating with GSearch.'),
      '#default_value' => $config->get('islandora_gsearcher_gsearch_pass'),
    ];
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => $this->t('Submit'),
    ];
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $user = $form_state->getValue('islandora_gsearcher_gsearch_user');
    $password = $form_state->getValue('islandora_gsearcher_gsearch_pass');
    $url = $form_state->getValue('islandora_gsearcher_gsearch_url');

    $client = $this->httpClient;
    try {
      $client->request('GET', "http://$user:$password@$url");
    }
    catch (RequestException $exception) {
      $status_code = $exception->getResponse()->getStatusCode();
      if ($status_code == 401) {
        $form_state->setErrorByName('', $this->t('Failed to authenticate with GSearch.'));
      }
      elseif ($status_code != 200) {
        $form_state->setErrorByName('', $this->t('GSearch did not return 200. Something may be wrong with your configuration or GSearch.'));
      }
    }
  }

}
