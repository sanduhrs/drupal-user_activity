<?php
/* $Id$ */

/**
 * @file
 * Calculates and displays a user's activity index.
 *
 * The calculation is based on logins, pis, created nodes and comments
 *
 * @author
 * Stefan Auditor <stefan.auditor@erdfisch.de>
 */

/**
 * Settings page
 */
function user_activity_settings() {
  $weighting = drupal_map_assoc(array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25));
  $form['user_activity'] = array(
    '#type' => 'fieldset',
    '#title' => t('Calculation options'),
    '#description' => t('Adjust the Options to influence the activity calculation.'),
    '#prefix' => '<div class="admin clear-block">',
    '#suffix' => '</div>',
  );
  $form['user_activity']['user_activity_timeframe'] = array(
    '#type' => 'select',
    '#title' => t('Timeframe'),
    '#default_value' => variable_get('user_activity_timeframe', 0),
    '#options' => array(
      0       => t('Unlimited'),
      604800  => t('One week'),
      1209600 => t('Two weeks'),
      1814400 => t('Three weeks'),
      2419200 => t('One month'),
      4838400 => t('Two month'),
      7257600 => t('Three months'),
      9676800 => t('Four months'),
      12096000 => t('Five months'),
      14515200 => t('Six months'),
    ),
    '#description' => t('The timeframe used for the activity calculation. Statistics older than this are auto deleted on cron.'),
    '#prefix' => '<div class="left clear-block">',
    '#suffix' => '</div>',
  );
  $form['user_activity']['user_activity_mode'] = array(
    '#type' => 'select',
    '#title' => t('Mode'),
    '#default_value' => variable_get('user_activity_mode', 'dynamic'),
    '#options' => array(
      'dynamic' => t('Dynamic mode'),
      'static' => t('Static mode'),
    ),
    '#description' => t('The dynamic activity index is calculated in comparison to the values the most active user has achieved. The static mode index is based on <a href="#user-activity-static-mode">the values given below</a>.'),
    '#prefix' => '<div class="right clear-block">',
    '#suffix' => '</div>',
  );
  $form['user_activity']['user_activity_lifetime'] = array(
    '#type' => 'select',
    '#title' => t('Lifetime'),
    '#default_value' => variable_get('user_activity_lifetime', 0),
    '#options' => array(
      0      => t("Don't cache"),
      60     => t('One minute'),
      300    => t('5 minutes'),
      900    => t('15 minutes'),
      1800   => t('30 minutes'),
      3600   => t('One hour'),
      7200   => t('Two hours'),
      21600  => t('6 hours'),
      43200  => t('12 hours'),
      64800  => t('18 hours'),
      86400  => t('24 hours'),
      172800 => t('48 hours'),
    ),
    '#description' => t('How long to cache the activity index.'),
    '#prefix' => '<div class="left clear-block">',
    '#suffix' => '</div>',
  );

  $form['user_activity']['user_activity_cron_items'] = array(
    '#type' => 'select',
    '#title' => t('Reindex on cron'),
    '#default_value' => variable_get('user_activity_cron_items', 25),
    '#options' => array(
      25   => t('25 items'),
      50   => t('50 items'),
      100  => t('100 items'),
      150  => t('150 items'),
      250  => t('250 items'),
      500  => t('500 items'),
      1000 => t('1000 items'),
    ),
    '#description' => t('Choose how many items to recalculate on cron.'),
    '#prefix' => '<div class="right clear-block">',
    '#suffix' => '</div>',
  );

  $form['user_activity_weighting'] = array(
    '#type' => 'fieldset',
    '#title' => t('Weighting'),
    '#description' => t('The weighting is used to give one value more influence than the other. Set heigher to increase and lower to decrease influence on the activity index.'),
    '#prefix' => '<div class="admin clear-block">',
    '#suffix' => '</div>',
  );
  $form['user_activity_weighting']['user_activity_pis'] = array(
    '#type' => 'select',
    '#title' => t('Page impressions'),
    '#default_value' => variable_get('user_activity_pis', 1),
    '#options' => $weighting,
    '#prefix' => '<div class="left clear-block">',
  );
  $form['user_activity_weighting']['user_activity_logins'] = array(
    '#type' => 'select',
    '#title' => t('User logins'),
    '#default_value' => variable_get('user_activity_logins', 1),
    '#options' => $weighting,
    '#suffix' => '</div>',
  );
  $form['user_activity_weighting']['user_activity_nodes'] = array(
    '#type' => 'select',
    '#title' => t('Nodes created'),
    '#default_value' => variable_get('user_activity_nodes', 1),
    '#options' => $weighting,
    '#prefix' => '<div class="right clear-block">',
  );
  $form['user_activity_weighting']['user_activity_comments'] = array(
    '#type' => 'select',
    '#title' => t('Comments created'),
    '#default_value' => variable_get('user_activity_comments', 1),
    '#options' => $weighting,
    '#suffix' => '</div>',
  );

  $form['user_activity_static_mode'] = array(
    '#type' => 'fieldset',
    '#title' => t('Static mode'),
    '#description' => t('If static mode is enabled, these values are used for the index calculation. '),
    '#prefix' => '<div id="user-activity-static-mode" class="admin clear-block">',
    '#suffix' => '</div>',
  );
  $timeframe = time() - variable_get('user_activity_timeframe', 0);
  $max['pis']      = user_activity_get_max_pis();
  $max['logins']   = user_activity_get_max_logins();
  $max['nodes']    = user_activity_get_max_nodes($timeframe);
  $max['comments'] = user_activity_get_max_comments($timeframe);
  $form['user_activity_static_mode']['user_activity_max_pis'] = array(
    '#type' => 'textfield',
    '#title' => t('Page impressions'),
    '#default_value' => variable_get('user_activity_max_pis', 100),
    '#size' => 15,
    '#description' => t("Page impressions per timeframe. Current most active user's value is %max", array('%max' => $max['pis'])),
    '#prefix' => '<div class="left clear-block">',
  );
  $form['user_activity_static_mode']['user_activity_max_logins'] = array(
    '#type' => 'textfield',
    '#title' => t('Logins'),
    '#default_value' => variable_get('user_activity_max_logins', 7),
    '#size' => 15,
    '#description' => t("User logins per timeframe. Current most active user's value is %max", array('%max' => $max['logins'])),
    '#suffix' => '</div>',
  );
  $form['user_activity_static_mode']['user_activity_max_nodes'] = array(
    '#type' => 'textfield',
    '#title' => t('Nodes'),
    '#default_value' => variable_get('user_activity_max_nodes', 5),
    '#size' => 15,
    '#description' => t("Nodes created per timeframe. Current most active user's value is %max", array('%max' => $max['nodes'])),
    '#prefix' => '<div class="right clear-block">',
  );
  $form['user_activity_static_mode']['user_activity_max_comments'] = array(
    '#type' => 'textfield',
    '#title' => t('Comments'),
    '#default_value' => variable_get('user_activity_max_comments', 25),
    '#size' => 15,
    '#description' => t("Comments created per timeframe. Current most active user's value is %max", array('%max' => $max['comments'])),
    '#suffix' => '</div>',
  );
  $form['#submit'][] = 'user_activity_settings_submit';
  return system_settings_form($form);
}

function user_activity_settings_submit($form_id, $form_values) {
  if ($form_values['values']['user_activity_mode'] != variable_get('user_activity_mode', 'dynamic')) {
    cache_clear_all('*', 'cache_user_activity', TRUE);
  }
}
