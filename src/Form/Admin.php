<?php

namespace Drupal\islandora_gsearcher\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use GuzzleHttp\ClientInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use GuzzleHttp\Exception\GuzzleException;
use Drupal\islandora\Utility\StateTrait;

/**
 * Module settings form.
 */
class Admin extends FormBase {
  use StateTrait;

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
  public static function stateDefaults() {
    return [
      'islandora_gsearcher_gsearch_url' => 'localhost:8080/fedoragsearch/rest',
      'islandora_gsearcher_gsearch_user' => 'fedoraAdmin',
      'islandora_gsearcher_gsearch_pass' => 'fedoraAdmin',
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $this->stateSetAll($form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $form['islandora_gsearcher_gsearch_url'] = [
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => $this->t('GSearch Address'),
      '#description' => $this->t('Endpoint to use when communicating with GSearch.'),
      '#default_value' => static::stateGet('islandora_gsearcher_gsearch_url'),
    ];
    $form['islandora_gsearcher_gsearch_user'] = [
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => $this->t('GSearch User'),
      '#description' => $this->t('User to use when communicating with GSearch.'),
      '#default_value' => static::stateGet('islandora_gsearcher_gsearch_user'),
    ];
    $form['islandora_gsearcher_gsearch_pass'] = [
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => $this->t('GSearch Password'),
      '#description' => $this->t('Password to use when communicating with GSearch.'),
      '#default_value' => static::stateGet('islandora_gsearcher_gsearch_pass'),
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
      $response = $client->request('GET', "http://$user:$password@$url");
      if ($response->getStatusCode() != 200) {
        $form_state->setErrorByName('', $this->t('GSearch did not return 200. Something may be wrong with your configuration or GSearch.'));
      }
    }
    catch (GuzzleException $exception) {
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
