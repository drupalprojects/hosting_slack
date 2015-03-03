<?php

/**
 * Implements hook_post_hosting_TASK_TYPE_task()
 */
function hosting_slack_post_hosting_deploy_task() {

}

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
    slack_send_message(t('Task started: [!title](!url)', array(
      '!title' => $task->title,
      '!url' => url("node/$task->nid", array('absolute' => TRUE)),
    ), $channel, $username));
  }
}