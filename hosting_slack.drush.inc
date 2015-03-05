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

    $message_options = new stdClass();
    $message_options->attachments = array();
    $message_options->attachments[0] = new stdClass();
    $message_options->attachments[0]->fallback = t('Task Started.');
    $message_options->attachments[0]->title = $task->title;
    $message_options->attachments[0]->title_link= url("node/$task->nid", array('absolute' => TRUE));
    $message_options->attachments[0]->text =  t('Task Started.');
    $message_options->attachments[0]->color = hosting_task_color($task->task_status);

    // Render task args as slackbot "fields"
    foreach ($task->task_args as $name => $value) {

      if (empty($value)) {
        continue;
      }
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
    slack_send_message('', $message_options);
  }
}

/**
 * Implements hook_hosting_task_update_status()
 */
function hosting_slack_hosting_task_update_status($task, $status) {

  $enabled_tasks = array_filter(variable_get('slack_hosting_tasks', array()));
  if (!isset($enabled_tasks[$task->task_type])){
    drush_log('Task type not configured to send notification.', 'ok');
    return;
  }

  $triggers = array_filter(variable_get('slack_hosting_tasks_triggers', array()));
  if ($triggers['stop']) {
    $status_text = _hosting_parse_error_code($status);
    $message_text =  t('Task Completed: !status', array(
      '!status' => "`{$status_text}`",
    ));
    $message_options = new stdClass();
    $message_options->attachments = array();
    $message_options->attachments[0] = new stdClass();
    $message_options->attachments[0]->fallback = t('Task Completed: !status', array(
      '!status' => $status_text,
    ));
    $message_options->attachments[0]->title = $task->title;
    $message_options->attachments[0]->text = $message_text;

    $message_options->attachments[0]->title_link= url("node/$task->nid", array('absolute' => TRUE));
    $message_options->attachments[0]->color = hosting_task_color($status);

    slack_send_message('', $message_options);
  }
}

function hosting_task_color($task_status) {
    $colors = array(
      HOSTING_TASK_SUCCESS => '#5cb85c',
      HOSTING_TASK_QUEUED => '#777',
      HOSTING_TASK_ERROR => '#d9534f',
      HOSTING_TASK_PROCESSING => '#aaa',
      HOSTING_TASK_WARNING => '#f0ad4e',
    );
    return $colors[$task_status];
}