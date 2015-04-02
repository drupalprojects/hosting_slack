<?php

/**
 * @file
 * Slack integration module API functions.
 */

/**
 * Send message to the Slack.
 *
 * @param string $message
 *   The message sent to the channel
 * @param string $channel
 *   The channel in the Slacksa service to send messages
 * @param string $username
 *   The bot name displayed in the channel
 *
 * @return bool|object
 *   Slack response.
 */
function slack_send_message($message, $message_options = NULL) {
  $team_name = variable_get('slack_team', '');
  if (!$team_name) {
    drupal_set_message('no team name');
    return FALSE;
  }
  $webhook_url =  variable_get('slack_webhook_url', '');
  if (!$webhook_url) {
    drupal_set_message('no webhook url');
    return FALSE;
  }

  // Invoke hook_hosting_slack_message_alter()
  drupal_alter('hosting_slack_message', $message_options);

  $result = _slack_send_message($team_name, $webhook_url, $message, $message_options);

  if ($result->code != 200){
    watchdog('hosting_slack', 'Slack POST unsuccessful: ' . $result->data);
    return FALSE;
  }
  else {
    return TRUE;
  }
}

/**
 * Send message to the Slack with more options.
 *
 * @param string $team_name
 *   Your team name in the Slack
 * @param string $webhook_url
 *   The token from "Incoming WebHooks" integration in the Slack
 * @param string $message
 *   The message sent to the channel
 * @param array $message_options
 *   An associative array, it can contain:
 *     - channel: The channel in the Slack service to send messages
 *     - username: The bot name displayed in the channel
 *
 * @return object
 *   Can contain:
 *                          success      fail          fail
 *     - data:                ok         No hooks      Invalid channel specified
 *     - status message:      OK         Not found     Server Error
 *     - code:                200        404           500
 *     - error:               -          Not found     Server Error
 */
function _slack_send_message($team_name, $webhook_url, $message, $message_options = NULL) {
  $headers = array(
    'Content-Type' => 'application/x-www-form-urlencoded',
  );

  if ($message_options == NULL) {
    $message_options = new stdClass();
  }
  $message_options->text = $message;

  $sending_data = 'payload=' . json_encode($message_options);
  $result = drupal_http_request($webhook_url, $headers, 'POST', $sending_data);
  return $result;
}
