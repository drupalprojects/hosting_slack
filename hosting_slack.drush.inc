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

    $tasks = hosting_available_tasks();
    $message = t('!type Task Started: *!date*', array(
      '!date' => format_date($task->executed),
      '!type' => $tasks[$task->task_type]['title'],
    ));
    $message_options = new stdClass();
    $message_options->attachments = array();
    $message_options->attachments[0] = new stdClass();
    $message_options->attachments[0]->fallback = $message;
    $message_options->attachments[0]->title = $task->title;
    $message_options->attachments[0]->title_link= url("node/$task->nid", array('absolute' => TRUE));
    $message_options->attachments[0]->text = $message;
    $message_options->attachments[0]->color = hosting_task_color($task->task_status);

    // Render task args as slackbot "fields"
    if (is_array($task->task_args)) {
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
        $message_options->attachments[0]->fields[$name] = $field;
      }
      slack_send_message('', $message_options);
    }
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
    $tasks = hosting_available_tasks();
    $message_text = t('!type Task Ended: !status !date', array(
      '!date' => format_date($task->executed),
      '!type' => $tasks[$task->task_type]['title'],
      '!status' => _hosting_parse_error_code($status),
    ));
    $message_options = new stdClass();
    $message_options->attachments = array();
    $message_options->attachments[0] = new stdClass();
    $message_options->attachments[0]->fallback = t('Task Completed: !status', array(
      '!status' => _hosting_parse_error_code($status),
    ));
    $message_options->attachments[0]->title = $task->title;
    $message_options->attachments[0]->text = $message_text;

    $message_options->attachments[0]->title_link= url("node/$task->nid", array('absolute' => TRUE));
    $message_options->attachments[0]->color = hosting_task_color($status);

    $field = new stdClass();
    $field->title = 'Duration';
    $field->value = format_interval($task->delta);
    $field->short = TRUE;
    $message_options->attachments[0]->fields[] = $field;

    $field = new stdClass();
    $field->title = 'Started';
    $field->value = format_date($task->executed);
    $field->short = TRUE;
    $message_options->attachments[0]->fields[] = $field;

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