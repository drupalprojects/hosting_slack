<?php

/**
 * @file
 * Slack integration module admin functions.
 */

/**
 * Slack module admin form.
 */
function slack_configure_form() {
  $form = array();
  $form['slack_team'] = array(
    '#type' => 'textfield',
    '#title' => t('Team name'),
    '#default_value' => variable_get('slack_team', ''),
    '#required' => TRUE,
  );
  $form['slack_webhook_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Webhook URL'),
    '#description' => t('Add the "Incoming WebHooks" integration in Slack and add the Webhook URL here.'),
    '#default_value' => variable_get('slack_webhook_url', ''),
    '#required' => TRUE,
  );
  $form['slack_channel'] = array(
    '#type' => 'textfield',
    '#title' => t('Default channel'),
    '#description' => t('Enter your channel name with # symbol, for example #general (or @username for a private message or a private group name).'),
    '#default_value' => variable_get('slack_channel', ''),
  );
  $form['slack_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Default username'),
    '#description' => t('What would you like to name your Slack bot?'),
    '#default_value' => variable_get('slack_username', ''),
  );
  return system_settings_form($form);
}

/**
 * Slack test message form.
 */
function slack_send_test_message_form() {
  $form = array();
  $form['slack_test_channel'] = array(
    '#type' => 'textfield',
    '#title' => t('Channel'),
    '#default_value' => variable_get('slack_channel'),
  );
  $form['slack_test_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#default_value' => variable_get('slack_username'),
  );
  $form['slack_test_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send message'),
  );
  return $form;
}

/**
 * Submit handler for slack test message form.
 */
function slack_send_test_message_form_submit($form, &$form_state) {
  $channel = $form_state['values']['slack_test_channel'];
  $username = $form_state['values']['slack_test_username'];
  $message = $form_state['values']['slack_test_message'];

  $result = slack_send_message($message, $channel, $username);
  if (!$result) {
    drupal_set_message(t("Message wasn't sent. Please, check slack module configuration."));
  }
  elseif (!isset($result->error) && $result->code == SLACK_CODE_OK) {
    drupal_set_message(t('Message was successfully sent.'));
  }
  else {
    drupal_set_message(t("Message wasn't sent."), 'error');
  }
}