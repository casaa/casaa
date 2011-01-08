<?php
/*****************************************************
 * File: casaa.global.inc (CASAA v: 6.x-1.0.0)
 * 
 * This include file contains the functions handling
 * the global settings for the framework and all 
 * global settings for each installed plugin
 *****************************************************/ 

/**
 * This is the page callback function that generates the form 
 * for handling global settings for the framework.
 * 
 * @return $output - a string containing themed/raw html for output.
 **/
function casaa_global_settings_page() {
	$output = '';
	$output .= '<h3>Global Settings</h3>';
	
	$output .= drupal_get_form('casaa_global_settings_form');
	
	return $output;
}

/**
 * Function builds the global settings form. First it creates fields
 * for the main global of the framework, itself and then it invokes
 * the global settings hook functions from the plugins to create the 
 * fields for each installed plugin.
 * 
 * @param $tokens - and array containing the currently installed plugins
 * 
 * @return $form - an array containing form elements.
 *  I am using drupal's system_settings_form function to store the global settings
 *  within drupal's variables db table to minimize db usage by the framework. 
 **/
function casaa_global_settings_form() {
	//$tokens = casaa_get_plugin_tokens();
	$tokens = casaa_get_installed_plugin_tokens();
	$form = array();

	$form['casaa_system_settings'] = array(
		'#type' => 'fieldset',
		'#title' => t('CASAA System Settings'),
		'#description' => t('Manage the system settings for the CASAA module'),
	);
		$form['casaa_system_settings']['path_term_select'] = array(
			'#type' => 'radios',
			'#title' => t('Mapping Type Options'),
			'#description' => t('Select which type of mapping you would like to use. Changing this will not affect existing settings only which type you may view, edit, and what gets output.'),
			'#options' => array('path and term' => t('Both Path and Term Mapping'), 'term only' => t('Term Mappings Only'), 'path only' => t('Path Mapping Only')),
			'#default_value' => variable_get('path_term_select', 'path and term'),
		);

		$form['casaa_system_settings']['casaa_wildcard'] = array(
			'#type' => 'checkbox',
			'#title' => t('Allow Wildcards'),
			'#description' => t("Allow users to use wildcards '*' when mapping URL paths."),
			'#default_value' => variable_get('casaa_wildcard', FALSE),
		);
		$term_only = variable_get('path_term_select', 'path and term');
		if ($term_only == 'term only') {
			$form['casaa_system_settings']['casaa_wildcard']['#disabled'] = TRUE;
		}
	if (empty($tokens)) {
		$description = "You have not enabled the sub-modules/plug-ins. Please refer to: " . l('Administer Modules', 'admin/build/modules') . " to enable the plug-ins you wish to use.";
	}
	else {
		$description = t('Manage the global settings for installed plugins');
	}
	$form['casaa_global_settings'] = array(
		'#type' => 'fieldset',
		'#title' => t('Plug-in Global Settings'),
		'#description' => $description,
	);
	if ($tokens == NULL) {
		return system_settings_form($form);
	}
		if ($tokens !== NULL) {
			foreach ($tokens as $token) {
				$form['casaa_global_settings'][$token . '_global_settings'] = array(
					'#type' => 'fieldset',
					'#title' => t(str_replace('_', ' ', $token)),
					'#collapsible' => TRUE,
					'#collapsed' => TRUE,
				);
				$hook_global_settings_form = $token . '_global_settings_form';
				if (function_exists($hook_global_settings_form)) {
					$form['casaa_global_settings'][$token . '_global_settings'][] = $hook_global_settings_form();
				}
			}
		}
	$form['#submit'] = array('casaa_global_settings_submit_handler');
	return system_settings_form($form);
}

/**
 * return only plugin tokens that are currently installed and enabled in the system.
 * this is to help users know that they need to enable the sub-modules/plug-ins.
 * 
 * @return $enabled_tokens an array listing the installed plug-ins that are enabled in 
 * drupal's module system table.
 **/

function casaa_get_installed_plugin_tokens() {
	$enabled_tokens = array();
	$tokens = casaa_get_plugin_tokens();
	foreach ($tokens as $token) {
		$query = db_query("SELECT status FROM {system} WHERE name='%s'", $token);
		$result = db_fetch_object($query);
		if ($result->status == 1) {
			$enabled_tokens[] = $token;
		}
	}
	if (empty($enabled_tokens)) {
		return NULL;
	}
	else {
		return $enabled_tokens;
	}
	
	
}

/**
 * callback to allow any plugins that need to alter the submitted data 
 * a chance to do so. Functions
 */
function casaa_global_settings_submit_handler($form, &$form_state) {

	$tokens = casaa_get_plugin_tokens();
	foreach ($tokens as $token)
	{
		if ($token) {	
			$global_settings_submit_handler = $token . '_global_settings_submit_handler';
			if (function_exists($global_settings_submit_handler)) {
				$global_settings_submit_handler($form, $form_state);
			}
		}
	}
}
