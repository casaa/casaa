<?php
/**
 * @file - edit current settings for an existing mapping
 */ 


/**
 * 
 */
function casaa_edit_settings_callback($js = FALSE) {
	$title = t('Edit Mapping Setting');
	
	ctools_include('ajax');
	ctools_include('modal');
	
	$output .= drupal_get_form('casaa_edit_settings_form');
	if ($js) {
		ctools_modal_render($title, $output);
	}

	return $output;
}

function casaa_edit_settings_form() {
	$form = array();
	
	$domain = arg(6);
	$type = arg(7);
	$id = arg(8);
	
	$plugins = _casaa_load_plugins();
	
	// load the defaults per plugin for the mapping
	foreach ($plugins as $plugin => $values) {
		$default_values[$plugin] = _casaa_load_defaults($plugin, $domain, $type, $id);
		$hook = $plugin . "_mapping_settings_form";
		if (function_exists($hook)) {
			$form['casaa_edit_settings'][] = $hook($default_values[$plugin]);
		}
	}
	$form['casaa_edit_settings']['mapping_values'] = array(
		'#type' => 'hidden',
		'#value' => array(
			'domain' => $domain,
			'type' => $type,
			'id' => $id,
		),
	);
	
	$form['casaa_edit_settings']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit'),
	);
	$form['casaa_edit_settings']['cancel'] = array(
		'#type' => 'submit',
		'#value' => t('Cancel'),
	);
	return $form;
}

/**
 * 
 */ 
function casaa_edit_settings_form_submit($form, $form_state) {
	ctools_include('ajax');
	ctools_include('modal');
	
	$commands = array();
	
	$op = $form_state['values']['op'];
	$base = $form_state['values']['mapping_values'];
	$plugins = _casaa_load_plugins();
	
	if ($op == 'Submit') {
		//do some stuff
		foreach ($plugins as $plugin => $values) {
			$saved = _casaa_update_values($base['type'], $base['domain'], $plugin, $base['id'], $form_state['values']);
		}
		$commands[] = ctools_modal_command_dismiss(t('w00t!'));
	}
	else if ($op == 'Cancel') {
		drupal_set_message('Edit Canceled');
		$commands[] = ctools_modal_command_dismiss(t('Canceling'));
	}
	ctools_ajax_render($commands);
} 

/**
 * Update the values for a mapping.
 * @param $plugin - given plugin to pull values for.
 * @param $domain - domain id for mappings
 * @param $type - 'term' or 'path'
 * @param $id - per type: 'term' = tid  'path' = aid
 * @param $values - form values
 * 
 * Let the plugin's submit handler package the data so, that we only have to 
 * serialize and update the row.
 */ 
function _casaa_update_values($type, $domain, $plugin, $id, $values) {
	$saved = FALSE;
	$hook = $plugin . "_mapping_settings_form_submit_handler";
	
	if (function_exists($hook)) {
		$pack = $hook($values);
	}
	
	if ($type == 'path') {
		$query = db_query("SELECT p.aid FROM {casaa_context_mapper} c 
			LEFT JOIN {casaa_paths} p ON c.aid=p.aid
			WHERE c.casaa_domain=%d AND c.casaa_type='%s' AND p.casaa_path=(
				SELECT casaa_path FROM {casaa_paths} WHERE aid=%d
			)
		", $domain, $plugin, $id);
		$aid = db_fetch_object($query);
		if (db_query("UPDATE {casaa_context_mapper} SET casaa_value='%s' WHERE aid=%d", $pack, $aid->aid)) {
			$saved = TRUE;
		}
	}
	else if ($type == 'term') {
		$query = db_query("SELECT c.aid FROM {casaa_context_mapper} c
			LEFT JOIN {casaa_terms} t ON c.aid=t.aid
			WHERE c.casaa_domain=%d AND c.casaa_type='%s' AND t.casaa_tid=%d
		", $domain, $plugin, $id);
		$aid = db_fetch_object($query);
		if (db_query("UPDATE {casaa_context_mapper} SET casaa_value='%s' WHERE aid=%d", $pack, $aid->aid)) {
			$saved = TRUE;
		}
	}
	return $saved;
}

/**
 * Load the default values for the domain mapping
 * given the type and id.
 * 
 * @param $plugin - given plugin to pull values for.
 * @param $domain - domain id for mappings
 * @param $type - 'term' or 'path'
 * @param $id - per type: 'term' = tid  'path' = aid
 */ 
function _casaa_load_defaults($plugin, $domain, $type, $id) {
	if ($type == 'path') {
		$query = db_query("SELECT c.casaa_value FROM {casaa_context_mapper} c
			LEFT JOIN {casaa_paths} p ON c.aid=p.aid
			WHERE c.casaa_domain=%d AND c.casaa_type='%s' AND p.casaa_path = (
				SELECT casaa_path FROM {casaa_paths} WHERE aid=%d
			)			
		", $domain, $plugin, $id);
		$value = db_fetch_object($query);
	}
	else if ($type == 'term') {
		$query = db_query("SELECT c.casaa_value FROM {casaa_context_mapper} c 
			LEFT JOIN {casaa_terms} t ON c.aid=t.aid
			WHERE t.casaa_tid=%d AND c.casaa_type='%s' AND c.casaa_domain=%d
		", $id, $plugin, $domain);
		
		$value = db_fetch_object($query);
	}
	
	return unserialize($value->casaa_value);
}