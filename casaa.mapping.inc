<?php
/*********************************************************
 * File: casaa.mapping.inc (CASAA v: 6.x-1.11-beta2)
 * 
 * This include file contains all of the functions for the
 * mapping settings page including all of the configuration
 * and deletion functions for mapping settings. 
 *********************************************************/


/**
 * This function is the main page callback function that builds
 * the settings tables displaying all current mapping settings
 * within the system according to what the user has set to use
 * in the global settings page.
 * 
 * @param $display_option - a string containing the display option set in the system settings.
 *  this option allows the user to choose between path settings, term settings or both.
 * @param $path_header - an array containing the labels for path mappings table.
 * @param $term_header - an array containgin the labels for term mappings table.
 * @param $tax_settings_row - an array containing the table rows of term settings.
 * @param $path_settings_row - an array containing the table rows of path settings.
 * 
 * @return $output - a string containing raw/themed html for output.
 **/ 
function casaa_mapping_settings_page() {
	//run a quick update function to make sure all url paths are stored w/out a leading slash.
	//this is to conform for the new mapping standard for the system.
	$ran = variable_get('casaa_db_updated', FALSE);
	if (!$ran) {
		if ($ran = casaa_update_db()) {
			variable_set('casaa_db_updated', $ran);
			drupal_set_message('CASAA Paths table has been updated.');
		}
		else {
			drupal_set_message("Could not update CASAA Paths table. Please see your administrator.", 'error');
		}
	}
	require(drupal_get_path('module', 'casaa') . '/casaa.filter.inc');
	$output = '';	
	
	$output .= drupal_get_form('casaa_filter_form');
	
	$display_option = variable_get('path_term_select', 'path and term');
		
		$output .= '<h2>Mapping Settings</h2>';
	
		$path_header = casaa_path_get_plugin_header_info();
		$tax_header = casaa_term_get_plugin_header_info();
		$limit = 7;
		$tax_settings_rows = casaa_get_mapping_settings('tax', $expanded = FALSE);
		$tax_settings_rows[] = array(
			'data' => array(
				//the $element parameter is the unique identifier for the pager that matches the $element identifier from the pager_query function
				array('data' => theme_pager(array(), $limit = 7, $element = 0, array(), $quantity = 9), 'class' => term_pager, 'colspan' => 7),
			)
		);
		$path_settings_rows = casaa_get_mapping_settings('path', $expanded = FALSE);
		$path_settings_rows[] = array(
			'data' => array(
				//the $element parameter is the unique identifier for the pager that matches the $element identifier from the pager_query function
				array('data' => theme_pager(array(), $limit = 7, $element = 1, array(), $quantity = 9), 'class' => path_pager, 'colspan' => 7)
				)
		);
	if ($display_option == 'path and term' || $display_option == 'term only') {
		$output .= 'Taxonomy Mappings';
		$output .= theme_table($tax_header, $tax_settings_rows);
		$output .= l(t('View All Terms'), 'admin/build/casaa/expanded_view/term');

		$output .= "<br><br><br>";
	}
	if ($display_option == 'path and term' || $display_option == 'path only') {
		$output .= 'Path Mappings';
		$output .= theme_table($path_header, $path_settings_rows);
		$output .= l(t('View All Paths'), 'admin/build/casaa/expanded_view/path' );
	}
	return $output;
}

/**
 * Gather the settings for both paths and terms from the db and build the 
 * columns and rows for the settings tables.
 * 
 * @param $type - string containing which type of settings to pull: path or term
 * @param $expanded - boolean that determines whether or not the view is expanded for the expanded view tables
 *  the expanded value determines the max number of rows per table page.
 * 
 * @return $rows - an array containing the columns and rows that will be themed through the theme_table function later
 **/
function casaa_get_mapping_settings($type = '', $expanded = FALSE) {

	$tokens = casaa_get_plugin_tokens_with_values();
	$rows = array();
	$limit = 7;
	if ($expanded) {
		$limit = 15;
	}	
	if ($type == 'tax') {
		if ((filter_xss($_GET['sort'] == 'asc') || filter_xss($_GET['sort'] == 'desc')) && $_GET['order'] == "Categories") {
			$sort = "ORDER BY t.name " . strtoupper($_GET['sort']);
		}
		$query = pager_query("SELECT DISTINCT c.casaa_tid FROM {casaa_terms} c LEFT JOIN {term_data} t ON c.casaa_tid=t.tid " . $sort, $limit, $element = 0, $count_query = "SELECT COUNT(DISTINCT(casaa_tid)) FROM casaa_terms");
		while ($key = db_fetch_object($query)) {
			$tax_keys[] = $key->casaa_tid;
		}
		if ($tax_keys) {
			foreach ($tax_keys as $key) {
				$columns = '';
				$term_name = taxonomy_get_term($key);
				$columns[] = $term_name->name;
				$query = db_query("SELECT c.casaa_weight FROM {casaa_context_mapper} c JOIN {casaa_terms} t ON c.aid = t.aid WHERE t.casaa_tid='%s'", $key);
				$term_weight = db_fetch_object($query);
				$columns[] = $term_weight->casaa_weight;
				foreach ($tokens as $token) {
					$query = db_query("SELECT c.aid, c.casaa_value FROM {casaa_context_mapper} c JOIN {casaa_terms} t ON c.aid = t.aid WHERE c.casaa_type='%s' AND t.casaa_tid='%s'", $token, $key);
					$value = db_fetch_object($query);
					$hook_table_display_data = $token . '_table_display_data';
					$display_value = '';
					if (function_exists($hook_table_display_data)) {
						$display_value = $hook_table_display_data($value->casaa_value);
					}
					
					$columns[] = $display_value;
				}
				$columns[] = l('configure', 'admin/casaa/edit/term/' . $key) . ' / ' . l('delete', 'admin/casaa/delete/term/' . $key);
				$rows[] = $columns;
			}
		}
	}
	if ($type == 'path') {
		if ((filter_xss($_GET['sort'] == 'asc') || filter_xss($_GET['sort'] == 'desc')) && $_GET['order'] == "Paths") {
			$sort = "ORDER BY casaa_path " . strtoupper($_GET['sort']);
		}
		$query = pager_query("SELECT DISTINCT casaa_path FROM {casaa_paths} " . $sort, $limit, $element = 1, $count_query = "SELECT COUNT(DISTINCT(casaa_path)) FROM casaa_paths");
		while ($path = db_fetch_object($query)) {
			$paths[] = $path->casaa_path;
		}
		if ($paths) {
			foreach ($paths as $path) {
				$columns = '';
				$columns[] = $path;
				foreach ($tokens as $token) {
					$query = db_query("SELECT c.aid, c.casaa_value FROM {casaa_context_mapper} c JOIN {casaa_paths} p ON c.aid = p.aid WHERE c.casaa_type='%s' AND p.casaa_path='%s'", $token, $path);
					$value = db_fetch_object($query);
					$hook_table_display_data = $token . '_table_display_data';
					$display_value = '';
					if (function_exists($hook_table_display_data)) {
						$display_value = $hook_table_display_data($value->casaa_value);
					}
					$columns[] = $display_value;
				}
				$query = db_query("SELECT aid FROM {casaa_paths} WHERE casaa_path='%s'", $path);
				$result = db_fetch_object($query);
				$columns[] = l('configure', 'admin/casaa/edit/path/' . $result->aid) . ' / ' . l('delete', 'admin/casaa/delete/path/' . $result->aid);
				$rows[] = $columns;
			}
		}
	}

	return $rows;
}

/**
 * This is the page callback function that generates the mapping configuration page
 * and the multi-step form that will guide the user through adding and editing data
 * for an existing mapping setting
 * 
 * @return $output - a string containing raw/themed html 
 **/
function casaa_edit_mapping_setting() {
	$output = '';
	$output .= '<h2>Configure Settings</h2>';
	$output .= drupal_get_form('casaa_edit_mapping_setting_form');
	return $output;
}

/**
 * Generate the form elements for the multi-step form to guide users through
 * the process of editing existing settings or adding new settings for an 
 * existing mapping
 * 
 * @param $form_state - array of values from the form when it was last rebuilt.
 * @param $default_values - an array containing the currently set values from a mapping
 * 
 * @return $form - an array containing form elements.
 **/
function casaa_edit_mapping_setting_form($form_state = NULL) {
	$form = array();
	$default_weight = casaa_get_default_weight($token, array('type' => arg(3), 'key' => arg(4)));
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
	
	if (!$form_state['my_data']['form_step']) {
		$form['casaa_edit_form'] = array(
			'#type' => 'fieldset',
		);
			$form['form_step'] = array(
				'#type' => 'hidden',
				'#value' => 0,
			);
			$form['casaa_edit_form']['select_option'] = array(
				'#type' => 'select',
				'#title' => t('What would you like to do?'),
				'#description' => t('Choose whether you would like to add new settings to the current configuration or edit the existing settings.'),
				'#options' => array('edit existing' => t('Edit Existing Settings'), 'add settings' => t('Add New Settings')),
				'#default_value' => 'edit existing',
			);
			$form['casaa_edit_form']['form_cancel'] = array(
				'#type' => 'submit',
				'#value' => t('Cancel'),
			);
			$form['casaa_edit_form']['form_next'] = array(
				'#type' => 'submit',
				'#value' => t('Next ->>'),
			);	
	}
	else if ($form_state['my_data']['form_step'] == 1) {
		$form['casaa_edit_form'] = array(
			'#type' => 'fieldset',
		);
		
		if ($form_state['my_data']['option_select'] == 'edit existing') {
			if (arg(3) == 'path') {
				$form['casaa_edit_form']['edit_path'] = array(
					'#type' => 'textfield',
					'#title' => t('URL Path'),
					'#description' => t('You may edit the path setting.'),
					'#default_value' => casaa_get_default_path(arg(3), arg(4)),
				);
			}
			else if (arg(3) == 'term') {
				$form['casaa_edit_form']['edit_weight'] = array(
					'#type' => 'select',
					'#title' => t('Weight'),
					'#options' => $weight_options,
					'#default_value' => (int) $default_weight,
				);
			}
			$tokens = casaa_get_plugin_tokens_with_values();
			foreach ($tokens as $token) {
				$default_values = casaa_get_default_values($token, array('type' => arg(3), 'key' => arg(4)));
				$hook_mapping_settings_form = $token . '_mapping_settings_form';
				if ($default_values != null && function_exists($hook_mapping_settings_form)) {
					$form['casaa_edit_form'][] = $hook_mapping_settings_form($default_values);
				}
			}
			$form['casaa_edit_form']['form_back'] = array(
				'#type' => 'submit',
				'#value' => t('<<- Back'),
			);
			$form['casaa_edit_form']['form_cancel'] = array(
				'#type' => 'submit',
				'#value' => t('Cancel'),
			);
			$form['casaa_edit_form']['form_save'] = array(
				'#type' => 'submit',
				'#value' => t('Save Settings'),
			);
		}
		else if ($form_state['my_data']['option_select'] == 'add settings') {
			$options = casaa_get_available_plugins($type = arg(3), $key = arg(4));
			$form['form_step'] = array(
				'#type' => 'hidden',
				'#value' => 1,
			);
			$form['casaa_edit_form']['plugin_select'] = array(
				'#type' => 'checkboxes',
				'#title' => t('Plugin Selection'),
				'#description' => t('Choose which plugins you would like to add settings for.'),
				'#options' => $options,
			);
			$form['casaa_edit_form']['form_back'] = array(
				'#type' => 'submit',
				'#value' => t('<<- Back'),
			);
			$form['casaa_edit_form']['form_cancel'] = array(
				'#type' => 'submit',
				'#value' => t('Cancel'),
			);
			$form['casaa_edit_form']['form_next'] = array(
				'#type' => 'submit',
				'#value' => t('Continue ->>'),
			);
		}
	}
	else if ($form_state['my_data']['form_step'] == 2) {
		$form['casaa_edit_form'] = array(
			'#type' => 'fieldset',
		);
			$form['plugin_select'] = array(
				'#type' => 'hidden',
				'#value' => $form_state['my_data']['plugin_select'],
			);
			foreach ($form_state['my_data']['plugin_select'] as $token) {
				$hook_mapping_settings_form = $token . '_mapping_settings_form';
				if (function_exists($hook_mapping_settings_form)) {					
					$form['casaa_edit_form'][] = $hook_mapping_settings_form();
				}
			}
			$form['casaa_edit_form']['form_back'] = array(
				'#type' => 'submit',
				'#value' => t('<<- Back'),
			);
			$form['casaa_edit_form']['form_cancel'] = array(
				'#type' => 'submit',
				'#value' => t('Cancel'),
			);
			$form['casaa_edit_form']['form_add_settings'] = array(
				'#type' => 'submit',
				'#value' => t('Add Settings'),
			);
	}	
	return $form;
}


/**
 * Take from the user's actions and determine at which point in the form
 * the user is.
 * 
 * @param $form_state - an array containing form values from the submitted form
 *   the "my_data" key is data from one step of the form that will be returned 
 *   to the form when it's rebuilt 
 **/
function casaa_edit_mapping_setting_form_submit($form, &$form_state) {
	if ($form_state['values']['op'] == 'Next ->>') {
		$form_state['my_data']['option_select'] = $form_state['values']['select_option'];
		$form_state['my_data']['form_step'] = $form_state['values']['form_step'];
		$form_state['my_data']['form_step']++;
		$form_state['rebuild'] = TRUE;
	}
	if ($form_state['values']['op'] == 'Continue ->>') {
		$form_state['my_data']['plugin_select'] = $form_state['values']['plugin_select'];
		$form_state['my_data']['form_step'] = $form_state['values']['form_step'];
		$form_state['my_data']['form_step']++;
		$form_state['rebuild'] = TRUE;
	}
	else if ($form_state['values']['op'] == 'Cancel') {
		drupal_set_message('Action Canceled');
		drupal_goto('admin/build/casaa/mapping_settings');
	}
	else if ($form_state['values']['op'] == 'Save Settings') {
		drupal_set_message('saving settings');
		casaa_edit_update_data($form_state['values'] , $key_arg = array('type' => arg(3), 'key' => arg(4)));
		drupal_goto('admin/build/casaa/mapping_settings');
	}
	else if ($form_state['values']['op'] == 'Add Settings') { 
		drupal_set_message('adding settings');

		casaa_edit_insert_data($form_state['values']['plugin_select'], $form_state['values'], $key_arg = array('type' => arg(3), 'key' => arg(4)));
		drupal_goto('admin/build/casaa/mapping_settings');
	}
}

/**
 * This is the page callback function for deleting a mapping setting./
 * 
 * @param $type - argument from the URI indicating path or term
 * @param $key - argument from the URI indicating the term id for term types and the aid for path types.
 * 
 * @return $output - a string containing raw/themed html for output.
 **/
function casaa_delete_mapping_setting() {
	$type = arg(3);
	$key = arg(4);
	$output = '';
	
	if ($type == 'term') {
		$term = taxonomy_get_term($key);
		$output .= "<h2>Do you really want to remove all mapping settings for the term:  \"" . $term->name . "\" ?</h2>";
	}
	else if ($type == 'path') {
		$query = db_query("SELECT casaa_path FROM {casaa_paths} WHERE aid='%s'", $key);
		$result = db_fetch_object($query);
		$path = $result->casaa_path;
		$output .= "<h2>Do you really want to remove all mapping settings for the URL path:  \"" . $path . "\" ?</h2>";
	}	
	$output .= drupal_get_form('casaa_delete_mapping_setting_form');
	return $output;
}


/**
 * Return a simple form to confirm the user's decision
 * 
 * @return $form - array containing form elements 
 **/
function casaa_delete_mapping_setting_form() {
	$form = array();
	
	$form['delete_form'] = array(
		'#type' => 'submit',
		'#value' => t('Delete'),
	);
	$form['cancel_form'] = array(
		'#type' => 'submit',
		'#value' => t('Cancel'),
	);
	return $form;
}

/**
 * determine the user's action and direct the user accordingly.  
 **/
function casaa_delete_mapping_setting_form_submit($form, &$form_state) {
	if ($form_state['values']['op'] == 'Cancel') {
		drupal_set_message('Action Canceled');
		drupal_goto('admin/build/casaa/mapping_settings');
	}
	else if ($form_state['values']['op'] == 'Delete') {
		$key_args = array('type' => arg(3), 'key' => arg(4));
		casaa_remove_settings($key_args);
	}
}

/**
 * Remove all settings values related to a path or term.
 * 
 * @param $deleted - boolean value indicating the success of the data removal. 
 **/
function casaa_remove_settings($key_args) {
	$deleted = FALSE;
	if ($key_args['type'] == 'term') {
		$query = db_query("SELECT aid FROM {casaa_terms} WHERE casaa_tid='%s'", $key_args['key']);
		while ($result = db_fetch_object($query)) {
			if (db_query("DELETE FROM {casaa_context_mapper} WHERE aid='%s'", $result->aid) && db_query("DELETE FROM {casaa_terms} WHERE aid='%s'", $result->aid)) {
				$deleted = TRUE;
			}
			else {
				$deleted = FALSE;
			}
		}
	}
	else if ($key_args['type'] == 'path') {
		$query = db_query("SELECT casaa_path FROM {casaa_paths} WHERE aid='%s'", $key_args['key']);
		$path = db_fetch_object($query);
		$query = db_query("SELECT aid FROM {casaa_paths} WHERE casaa_path='%s'", $path->casaa_path);
		while ($result = db_fetch_object($query)) {
			if (db_query("DELETE FROM {casaa_context_mapper} WHERE aid='%s'", $result->aid) && db_query("DELETE FROM {casaa_paths} WHERE aid='%s'", $result->aid)) {
				$deleted = TRUE;
			}
			else {
				$deleted = FALSE;
			}
		}
	}
	($deleted) ? drupal_set_message("Settings Successfully Removed") : drupal_set_message("Unable To Remove Settings", 'error');
	drupal_goto('admin/build/casaa/mapping_settings');
}

/**
 * Take new values from the edit form and insert them into the db.
 * @param $tokens - array containing a list of plugins to have values added to
 * @param $form_values - array containing the values to be inserted per token.
 * @param $key_arg - array with arguments 
 **/
function casaa_edit_insert_data($tokens, $form_values, $key_arg = array('type' => NULL, 'key' => NULL)) {
	drupal_set_message('inserting data');
	if (!in_array(NULL, $key_arg)) {
		foreach ($tokens as $token) {
			//make sure the $token value is not NULL or 0.
			//Those NULL or 0 values create dead weight (empty rows) in the db. not good. not good at all...
			if ($token) {	
				$hook_mapping_settings_submit_handler = $token . '_mapping_settings_submit_handler';
				if (function_exists($hook_mapping_settings_submit_handler)) {
					$adjusted_values[$token] = $hook_mapping_settings_submit_handler($form_values);
				}
				if ($key_arg['type'] == 'term') {
					if (db_query("INSERT INTO {casaa_context_mapper} (casaa_type, casaa_value) VALUES ('%s', '%s')", $token, $adjusted_values[$token])) {
						$query = db_query("SELECT aid FROM {casaa_context_mapper} ORDER BY aid DESC LIMIT 1");
						$aid = db_fetch_object($query);
						if (db_query("INSERT INTO {casaa_terms} (aid, casaa_tid) VALUES ('%s', '%s')", $aid->aid, $key_arg['key'])) {
							drupal_set_message('Settings Saved');
						}
						else {
							drupal_set_message('Unable To Save Settings', 'error');
						}
					}
				}
				else if ($key_arg['type'] == 'path') {
					if (db_query("INSERT INTO {casaa_context_mapper} (casaa_type, casaa_value) VALUES ('%s', '%s')", $token, $adjusted_values[$token])) {
						$sub_query = db_query("SELECT casaa_path FROM {casaa_paths} WHERE aid='%s'", $key_arg['key']);
						$path = db_fetch_object($sub_query);
						$query = db_query("SELECT aid FROM {casaa_context_mapper} ORDER BY aid DESC LIMIT 1");
						$aid = db_fetch_object($query);
						if (db_query("INSERT INTO {casaa_paths} (aid, casaa_path) VALUES ('%s', '%s')", $aid->aid, $path->casaa_path)) {
							drupal_set_message('Settings Saved');
						}
						else {
							drupal_set_message('Unable To Save Settings', 'error');
						}
					}
				}
			}
		}
	}
}

/**
 * Update all current values for an existing setting.
 * 
 * @param $form_values - array containing the values to be inserted per token.
 * @param $key_arg - array with arguments
 **/
function casaa_edit_update_data($form_values, $key_arg = array('type' => NULL, 'key' => NULL)) {
	$tokens = casaa_get_plugin_tokens_with_values();
	if (!in_array(NULL, $key_arg)) {
		
		$query = db_query("SELECT casaa_path FROM {casaa_paths} WHERE aid='%s'", $key_arg['key']);
		$path = db_fetch_object($query);
		
		foreach ($tokens as $token) {
			$hook_mapping_settings_submit_handler = $token . '_mapping_settings_submit_handler';
			if (function_exists($hook_mapping_settings_submit_handler)) {
				$adjusted_values[$token] = $hook_mapping_settings_submit_handler($form_values);
			}
			if ($key_arg['type'] == 'term') {
				$sub_query = db_query("SELECT c.aid FROM {casaa_terms} t JOIN {casaa_context_mapper} c ON t.aid = c.aid WHERE c.casaa_type='%s' AND t.casaa_tid='%s'", $token, $key_arg['key']);
				$aid = db_fetch_object($sub_query);
				if (db_query("UPDATE {casaa_context_mapper} SET casaa_value='%s', casaa_weight='%d' WHERE aid='%s'", $adjusted_values[$token], $form_values['edit_weight'], $aid->aid)) {
					drupal_set_message('Settings Updated');
				}
				else {
					drupal_set_message('Unable To Update Settings', 'error');
				}
			}
			else if ($key_arg['type'] == 'path') {
				
				$query = db_query("SELECT c.aid FROM {casaa_paths} p LEFT JOIN {casaa_context_mapper} c ON c.aid = p.aid WHERE c.casaa_type='%s' AND p.casaa_path='%s'", $token, $path->casaa_path);
				
				$aid = db_fetch_object($query);
				if ($form_values['edit_path'] !== $path->casaa_path) {
					db_query("UPDATE {casaa_paths} SET casaa_path='%s' WHERE aid='%s'", casaa_remove_leading_slash($form_values['edit_path']), $aid->aid);
				}
				if (db_query("UPDATE {casaa_context_mapper} SET casaa_value='%s', casaa_weight='%d' WHERE aid='%s'", $adjusted_values[$token], $form_values['edit_weight'], $aid->aid)) {
					drupal_set_message('Settings Updated');
				}
				else {
					drupal_set_message('Unable To Update Settings', 'error');
				}
			}
		}
	}
}

/**
 * For an existing setting get a list of plugins that do not have values
 * @param $type_key - indicates a path or term type
 * @param $key - is the tid for a term type and the aid for a path type.
 * 
 * @return $available_tokens - an array containing a list of plugins without values for an existing mapping setting 
 **/
function casaa_get_available_plugins($type_key = NULL, $key = NULL) {
	$available_tokens = array();
	if ($type_key == 'term') {
		$tokens = casaa_get_plugin_tokens();
		$query = db_query("SELECT c.casaa_type FROM {casaa_context_mapper} c JOIN {casaa_terms} t ON c.aid = t.aid WHERE t.casaa_tid='%s'", $key);
		while ($type = db_fetch_object($query)) {
			$types[] = $type->casaa_type;
		}
		foreach ($tokens as $token) {
			if (!in_array($token, $types)) {
				$available_tokens[$token] = $token;
			}
		}
	}
	else if ($type_key == 'path') {
		$tokens = casaa_get_plugin_tokens();
		$query = db_query("SELECT casaa_path FROM {casaa_paths} WHERE aid='%s'", $key);
		$path = db_fetch_object($query);
		$query = db_query("SELECT c.casaa_type FROM {casaa_context_mapper} c JOIN {casaa_paths} t ON c.aid = t.aid WHERE t.casaa_path='%s'", $path->casaa_path);
		while ($type = db_fetch_object($query)) {
			$types[] = $type->casaa_type;
		}
		foreach ($tokens as $token) {
			if (!in_array($token, $types)) {
				$available_tokens[$token] = $token;
			}
		}
	}
	return $available_tokens;
}

/**
 * Retreive the values for an existing mapping setting
 * @param $token - name of the plugin to retrieve values for.
 * @param $key_arg - array containing the arguments for key and type.
 * 
 * @return $default_values - an object containing the db results of the values for the plugin. 
 **/
function casaa_get_default_values($token, $key_arg) {
	if ($key_arg['type'] == 'term') {
		$query = db_query("SELECT c.casaa_value FROM {casaa_context_mapper} c JOIN {casaa_terms} t ON c.aid = t.aid WHERE t.casaa_tid='%s' AND c.casaa_type='%s'", $key_arg['key'], $token);
	}
	else if ($key_arg['type'] == 'path') {
		$sub_query = db_query("SELECT casaa_path FROM {casaa_paths} WHERE aid='%s'", $key_arg['key']);
		$sub_result = db_fetch_object($sub_query);
		$query = db_query("SELECT c.casaa_value FROM {casaa_context_mapper} c JOIN {casaa_paths} p ON c.aid = p.aid WHERE p.casaa_path='%s' AND c.casaa_type='%s'", $sub_result->casaa_path, $token);
	}
	$default_values = db_fetch_object($query);
	return $default_values->casaa_value;
}

/**
 * 
 **/
function casaa_get_default_path($type, $key) {
	$query = db_query("SELECT casaa_path FROM {casaa_paths} WHERE aid=%d", $key);
	$result = db_fetch_object($query);
	return $result->casaa_path;
}


/**
 * 
 **/
function casaa_get_default_weight($token, $key_arg) {
	$query = db_query("SELECT casaa_weight FROM {casaa_context_mapper} WHERE aid='%s'", $key_arg['key']);
	$result = db_fetch_object($query);
	return $result->casaa_weight;
}

/**
 *  
 **/
function casaa_path_get_plugin_header_info() {
	$tokens = casaa_get_plugin_tokens_with_values();
	if (!$tokens) {
		$tokens = array();
	}
	$header_row[] = array(
		'data' => t('Paths'),
		'field' => 'casaa_path',
		'sort' => 'desc',
	);
	foreach ($tokens as $token) {
		$header_row[] = $token;
	}
	$header_row[] = t('Options');
	return $header_row;	
}

function casaa_term_get_plugin_header_info() {
	$tokens = casaa_get_plugin_tokens_with_values();
	if (!$tokens) {
		$tokens = array();
	}
	$header_row[] = array(
		'data'  => t('Categories'),
		'field' => 'name',
		'sort'  => 'asc',
	);
	$header_row[] = t('Weight');
	foreach ($tokens as $token) {
		$header_row[] = str_replace('_', '', $token);
	}
	$header_row[] = t('Options');
	return $header_row;
}
