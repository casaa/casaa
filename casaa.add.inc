<?php
/********************************************************
 * File: casaa.add.inc (CASAA v: 6.x-1.0.0-dev)
 * 
 * This include file contains all functions related to 
 * adding settings in the framework.
 ********************************************************/

/**
 * This is the page callback function to generate a
 * multi-step form for adding a mapping setting
 * 
 * @return $output - a string containing themed or raw html for output 
 **/
function casaa_add_mapping_page() {
	$output = '';
	
	$output .= '<h3>Add Settings</h3>';
	$output .= "Choose Type To Begin";
	$output .= drupal_get_form('casaa_add_mapping_form');
	return $output;
}


/**
 * This is a multi-step form directing a user through the process
 * of adding a new mapping setting to the framework.
 * 
 * @param $tokens - an array of currently installed plugins
 * @param $form_state - the returned form state containing values to be used for each form step.
 *  I'm using the key "my_data" to signify data that is passed back from the form submission
 *  function. The values include form values that are to be used per step and a form_step value
 *  that is used to indicate at what point in the form the user is.
 * @param $vocabulary_list_options - an array containing the available vocabularies in Drupal's taxonomy.
 * @param $taxonomy_list_options - an array containing the taxonomy terms from the selected vocabulary.
 * @param $token_picks - an array of the plugins that the user selected for input in the previous step.
 *  
 * @return $form - an array containing form elements according to the Drupal Form API 
 **/
function casaa_add_mapping_form($form_state = NULL) {

	if (!$form_state['my_data']['step']) {
		$form['casaa_add_setting_form'] = array(
			'#type' => 'fieldset',
		);		
		$form['casaa_add_setting_form']['type_select'] = array(
			'#type' => 'select',
			'#title' => t('Choose Mapping Type'),
			'#description' => t('Choose whether you would like to add an URL path setting or a taxonomy term mapping.'),
			'#options' => array('none' => t('None'), 'path' => t('Path Mapping'), 'term' => t('Taxonomy Term Mapping')),
			'#default_value' => 'none',
		);
		$tokens = casaa_get_plugin_tokens();
		foreach ($tokens as $token) {
			$options[$token] = t($token);
		}
		$form['casaa_add_settings_form'] = array(
			'#type' => 'fieldset',
		);
		$form['casaa_add_settings_form']['plugin_select'] = array(
			'#type' => 'checkboxes',
			'#title' => t('Plugin Select'),
			'#description' => t('Select the plugins that you would like to add settings for.'),
			'#options' => $options,
		);
		$form['cancel'] = array(
			'#type' => 'submit',
			'#value' => t('Cancel'),
		);
		$form['continue'] = array(
			'#type' => 'submit',
			'#value' => t('Next ->>'),
		);
		$form_state['my_data']['step'] = 0;
	}
	if (!empty($form_state) && $form_state['my_data']['step'] == 1) {
		if ($form_state['values']['type_select'] == 'term') {
			$vocabulary_list_options = casaa_get_vocabulary_list_options();
			$taxonomy_list_options = casaa_get_taxonomy_list_options();
			$weight_options = array(
				'-10' => '-10',
				'-9' => '-9',
				'-8' => '-8',
				'-7' => '-7',
				'-6' => '-6',
				'-5' => '-5',
				'-4' => '-4',
				'-3' => '-3',
				'-2' => '-2',
				'-1' => '-1',
				'0' => '0',
				'1' => '1',
				'2' => '2',
				'3' => '3',
				'4' => '4',
				'5' => '5',
				'6' => '6',
				'7' => '7',
				'8' => '8',
				'9' => '9',
				'10' => '10');
			$form['casaa_mapping_settings']['casaa_term_settings'] = array(
				'#type' => 'fieldset',
				'#title' => t('Term Selection'),
				'#description' => t('Choose a term to map settings to'),
			);
				$form['casaa_mapping_settings']['casaa_term_settings']['mapping_setting_weight'] = array(
					'#type' => 'select',
					'#title' => t('Setting Weight'),
					'#options' => $weight_options,
					'#default_value' => 0,
				);
				$form['casaa_mapping_settings']['casaa_term_settings']['vocabulary_select'] = array(
					'#type' => 'select',
					'#title' => t('Vocabulary'),
					'#description' => t('Select the vocabulary of terms to choose from'),
					'#options' => $vocabulary_list_options,
					'#default_value' => variable_get('vocabulary_select', 0),
				);
				$form['casaa_mapping_settings']['casaa_term_settings']['change_vocab'] = array(
					'#type' => 'submit',
					'#value' => t('Change Vocabulary'),
				);
				$form['casaa_mapping_settings']['casaa_term_settings']['term_select'] = array(
					'#type' => 'select',
					'#title' => t('Category'),
					'#description' => t('Select the term to map to'),
					'#options' => $taxonomy_list_options,
					'#default_value' => variable_get('term_select', 0),
				);			
		}
		else if ($form_state['values']['type_select'] == 'path') {
			$form['casaa_mapping_settings']['casaa_path_settings'] = array(
				'#type' => 'fieldset',
				'#title' => t('URL Path'),
			);
				$form['casaa_mapping_settings']['casaa_path_settings']['casaa_path_field'] = array(
					'#type' => 'textfield',
					'#title' => t('URL Path'),
					'#description' => t('Enter a URL Path to map your settings to for example: "news/local" To create a mapping for the frontpage use "[front]". To map 404 pages use "[404]"'),
				);
		}
		
		$tokens = casaa_get_plugin_tokens();
		foreach ($tokens as $token) {
			if ($form_state['values']['plugin_select'][$token] ) {
				$token_picks[] = $token;
			}
		}
		$form['casaa_add_settings_form']['token_picks'] = array(
			'#type' => 'hidden',
			'#value' => $token_picks,
		);
		$form['casaa_add_settings_form'][] = casaa_build_plugin_mapping_forms($token_picks);
		$form['back'] = array(
			'#type' => 'submit',
			'#value' => t('<<- Back'),
		);
		$form['save'] = array(
			'#type' => 'submit',
			'#value' => t('Save Settings'),
		);
		$form['cancel'] = array(
			'#type' => 'submit',
			'#value' => t('Cancel'),
		);
	}
	
	return $form;
}


/**
 * Validating the user's selection from the first form step to
 * make sure they actually selected a mapping type.
 * @param $form - an array containing all elements of the form being submitted
 * @param $form_state - an array containing all values from the submitted form to be validated
 * 
 * @return - return FALSE is the user did not select from either option. 
 **/
function casaa_add_mapping_form_validate($form, &$form_state) {
	if ($form_state['values']['type_select'] == 'none') {
		//drupal_set_message('You must select a mapping type!', 'error');
		form_set_error( "type_select",'You must select a mapping type');
		//drupal_goto('admin/build/casaa/add_setting');
		return FALSE;
	}
}

/**
 * This function directs the actions from the multi-step form and rebuild the form
 * form according to which step in the form the user is.
 * @param $form - an array containing the form elements of the form being submitted.
 * @param $form_state - an array containing the values from the currently submitted form.
 *  The form's state may vary pending on which step in the form the user may be in.
 **/
function casaa_add_mapping_form_submit($form, &$form_state) {
	if ($form_state['values']['op'] == "<<- Back") {
		$form_state['rebuild'] = TRUE;
		$form_state['my_data']['step']--;
		$form_state['my_data']['value'] = $form_state['values'];
	}
	else if ($form_state['values']['op'] == "Next ->>") {
		$form_state['rebuild'] = TRUE;
		$form_state['my_data']['step']++;
		$form_state['my_data']['value'] = $form_state['values'];
	}
	else if ($form_state['values']['op'] == "Cancel") {
		drupal_set_message('Action Canceled');
		drupal_goto('admin/build/casaa/global_settings');
	}
	else if ($form_state['values']['op'] == "Save Settings") {
		//var_dump($form_state['values']['token_picks']);
		casaa_insert_data($form_state['values']['token_picks'], $form_state['values']);
	}
	else if ($form_state['values']['op'] == "Change Vocabulary") {
		variable_set('vocabulary_select', $form_state['values']['vocabulary_select']);
		$form_state['rebuild'] = TRUE;
		drupal_set_message('Changing Vocabulary');
	}
}


/**
 * Take incoming form data and pass it to the plugins for any custom
 * processing and then insert the data in the data base
 * 
 * @param $tokens  - array with the names of the plugins that have incoming values
 * @param $form_values - an array with values from the submitted form
 * 
 * @return $saved - a boolean value saying whether or not the data was saved 
 * properly 
 **/
function casaa_insert_data($tokens, $form_values) {
	$saved = FALSE;	
	//var_dump($form_values);
	foreach ($tokens as $token) {
		$hook_mapping_settings_submit_handler = $token . '_mapping_settings_submit_handler';
		if (function_exists($hook_mapping_settings_submit_handler)) {
			$adjusted_values[$token] = $hook_mapping_settings_submit_handler($form_values);
			if (!$form_values['mapping_setting_weight']) {
				!$form_values['mapping_setting_weight'] = 1;
			}
			if (($form_values['term_select'] || $form_values['casaa_path_field']) && db_query("INSERT INTO {casaa_context_mapper} (casaa_type, casaa_value, casaa_weight) VALUES ('%s', '%s', '%s')", $token, $adjusted_values[$token], $form_values['mapping_setting_weight'])) {
				$query = db_query("SELECT aid FROM {casaa_context_mapper} ORDER BY aid DESC LIMIT 1");
				$result = db_fetch_object($query);
				if ($form_values['term_select'] && db_query("INSERT INTO {casaa_terms} (aid, casaa_tid) VALUES ('%s', '%s')" ,$result->aid, $form_values['term_select'])) {
					$saved = TRUE;
				}
				else if ($form_values['casaa_path_field'] && db_query("INSERT INTO {casaa_paths} (aid, casaa_path) VALUES ('%s','%s')", $result->aid, casaa_remove_leading_slash($form_values['casaa_path_field']))) {
					$saved = TRUE;
				}
			}
			else {
				$saved = FALSE;
			}
		}
	}
	return $saved;
}

/**
* @param $vocab_option integer value of the selection
* @param $vocab_options asn array of category vocab names
*
* @todo adjust select output array to show parent/child relations visually.
*
*	return an array of taxonomy terms based on the currently selected vocabulary
*
* @return $terms the array of terms from the selected vocabulary
*/
function casaa_get_taxonomy_list_options() {
	$vocab_option = variable_get('vocabulary_select', 0);
	$vocab_options = casaa_get_vocabulary_list_options();
	$result = db_fetch_object(db_query("SELECT vid FROM {vocabulary} WHERE name='%s'", $vocab_options[$vocab_option]));
	$query = db_query("SELECT tid, name FROM {term_data} WHERE vid='%s'", $result->vid);	
	while ($term = db_fetch_object($query)) {
		$terms[$term->tid] = $term->name;
	}
	if ($terms) {
		return $terms;
	}else {	
		$terms = array(t('No Terms Available'));
		return $terms;
	}
}


/**
* @param $options a list of category vocabularies.
* 
* get a list of current vocabularies.
*
* @return $options an array of the category vocabulary names
*/

function casaa_get_vocabulary_list_options() {
	$options = array();
	$query = db_query("SELECT name FROM {vocabulary}");
	while ($option = db_fetch_object($query)) {
		$options[] = $option->name;
	}
	if ($options) {
		return $options;
	}else {
		$options = array(t('No Vocabularies Available'));
		return $options;
	}
}

/**
 * Invoke the hook function from each plugin to build the necessary
 * form elements and return the form array.
 * 
 * @return $form - array containing the form elements generated from each plugin. 
 **/
function casaa_build_plugin_mapping_forms($tokens) {
	$form = array();
	if ($tokens) {
		foreach ($tokens as $token) {
			$hook_token_mapping_settings_form = $token . '_mapping_settings_form';
			if (function_exists($hook_token_mapping_settings_form)) {
				$form[] = $hook_token_mapping_settings_form();
			}
		}
	}
	return $form;
}