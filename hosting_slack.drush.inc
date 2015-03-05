<?php

/**
 * Implements drush_HOOK_pre_COMMAND()
 */
function drush_hosting_slack_pre_hosting_task() {

  $task =& drush_get_context('HOSTING_TASK');

  $enabled_tasks = array_filter(variable_get('slack_hosting_tasks', array()));
  if (!isset($enabled_tasks[$task->task_type])){
    drush_log('Task type not configured to send notification.', 'ok');
    return;
  }

  $triggers = variable_get('slack_hosting_tasks_triggers', array());

  if ($triggers['start']) {

    $message_text =  t('Task Started: <!url|!title>', array(
      '!title' => $task->title,
      '!url' => url("node/$task->nid", array('absolute' => TRUE)),
    ));
    $message_options = new stdClass();
    $message_options->attachments = array();
    $message_options->attachments[0] = new stdClass();
    $message_options->attachments[0]->fallback = 'TASK OPTIONS GO HERE?';
    $message_options->attachments[0]->color = '#777';
    $message_options->attachments[0]->fields = array();

    // Render task args as slackbot "fields"
    foreach ($task->task_args as $name => $value) {

      if ($value == '0') {
        $value = 'no';
      }
      if ($value == '1') {
        $value = 'yes';
      }

      $field = new stdClass();
      $field->title = $name;
      $field->value = $value;
      $field->short = TRUE;

      $message_options->attachments[0]->fields[] = $field;
    }

    slack_send_message($message_text, $message_options);
  }
}

/**
 * Implements drush_HOOK_pre_COMMAND()
 */
function hosting_slack_post_hosting_task($task, $data) {

  drush_log('HELLO DRUSH!', 'ok');

  $enabled_tasks = variable_get('slack_hosting_tasks', array());

  if (!in_array($task->task_type, $enabled_tasks)){
    drush_log('Task type not configured to send notification.', 'ok');
    return;
  }

  $triggers = variable_get('slack_hosting_tasks_triggers', array());
  if ($triggers['stop']) {

    $message_text =  t('Task Complete: !status <!url|!title>', array(
      '!status' => _hosting_parse_error_code($task->task_status),
      '!title' => $task->title,
      '!url' => url("node/$task->nid", array('absolute' => TRUE)),
    ));
    $message_options = new stdClass();
    $message_options->attachments = array();
    $message_options->attachments[0] = new stdClass();
    $message_options->attachments[0]->fallback = $message_text;
    $message_options->attachments[0]->text = 'Details? ' . print_r($task, 1);;
    $message_options->attachments[0]->color = $task->task_status == HOSTING_TASK_SUCCESS? '#00cc00': '#cc0000';

    slack_send_message($message_text, $message_options);
  }
}