<?php

/**
 * Implements drush_HOOK_pre_COMMAND()
 */
function drush_hosting_slack_pre_hosting_task() {

  drush_log('HELLO DRUSH!', 'ok');

  $task =& drush_get_context('HOSTING_TASK');

  $enabled_tasks = variable_get('slack_hosting_tasks', array());

  if (!in_array($task->task_type, $enabled_tasks)){
    drush_log('Task type not configured to send notification.', 'ok');
    return;
  }

  $triggers = variable_get('slack_hosting_tasks_triggers', array());
  $channel = variable_get('slack_channel', '');
  $username = variable_get('slack_username', '');

  if ($triggers['start']) {

    $message_text = t('<!url|!title>', array(
      '!title' => $task->title,
      '!url' => url("node/$task->nid", array('absolute' => TRUE)),
    ));
    $message_options = new stdClass();
    $message_options->text = t('Task Started:');
    $message_options->attachments = array();
    $message_options->attachments[0] = new stdClass();
    $message_options->attachments[0]->fallback = $message_text;
    $message_options->attachments[0]->text = $message_text;
    $message_options->attachments[0]->color = '#777';

    slack_send_message($message_text, $channel, $username, $message_options);
  }
}

/**
 * Implements drush_HOOK_pre_COMMAND()
 */
function drush_hosting_slack_post_hosting_task() {

  drush_log('HELLO DRUSH!', 'ok');

  $task =& drush_get_context('HOSTING_TASK');

  $enabled_tasks = variable_get('slack_hosting_tasks', array());

  if (!in_array($task->task_type, $enabled_tasks)){
    drush_log('Task type not configured to send notification.', 'ok');
    return;
  }

  $triggers = variable_get('slack_hosting_tasks_triggers', array());
  $channel = variable_get('slack_channel', '');
  $username = variable_get('slack_username', '');

  if ($triggers['stop']) {

    $message_text = t('!status <!url|!title>', array(
      '!status' => _hosting_parse_error_code($task->task_status),
      '!title' => $task->title,
      '!url' => url("node/$task->nid", array('absolute' => TRUE)),
    ));
    $message_options = new stdClass();
    $message_options->text = t('Task Complete:');
    $message_options->attachments = array();
    $message_options->attachments[0] = new stdClass();
    $message_options->attachments[0]->fallback = $message_text;
    $message_options->attachments[0]->text = $message_text;
    $message_options->attachments[0]->color = $task->task_status == HOSTING_TASK_SUCCESS? '#00cc00': '#cc0000';

    slack_send_message($message_text, $channel, $username, $message_options);
  }
}