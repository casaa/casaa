<?php

/**
 * Delete the mapping settings row.
 */
function casaa_delete_settings_callback($js = FALSE) {
	$title = t('Delete Mapping');
	
	ctools_include('ajax');
  ctools_include('modal');
	
	$domain = arg(6);
	$type = arg(7);
	$id = arg(8); //id is based on type... tid for term, aid for path
	
	$output .= "Are you sure you wish to remove this setting?";
	$output .= drupal_get_form('casaa_delete_settings_form');
	if ($js) {
		ctools_modal_render($title, $output);
	} 
}

function casaa_delete_settings_form() {
	$values = array(
		'domain' => arg(6),
		'type' => arg(7),
		'id' => arg(8),
	);
	
	$form['casaa_delete_value'] = array(
		'#type' => 'hidden',
		'#value' => $values,
	);
	
	$form['casaa_delete_submit'] = array(
		'#type' => 'submit',
		'#value' => t('Delete'),
	);
	$form['casaa_delete_cancel'] = array(
		'#type' => 'submit',
		'#value' => t('Cancel'),
	);
	
	return $form;
}

function casaa_delete_settings_form_submit($form, $form_state) {
	ctools_include('ajax');
  ctools_include('modal');

	if ($form_state['values']['op'] == 'Delete') {
		if ($form_state['values']['casaa_delete_value']['type'] == 'term') {
			$query = db_query("SELECT c.aid FROM {casaa_context_mapper} c
				LEFT JOIN {casaa_terms} t ON c.aid = t.aid
				WHERE t.casaa_tid=%d
				", $form_state['values']['casaa_delete_value']['id']);
			while ($aid = db_fetch_object($query)) {
				if (db_query("DELETE FROM {casaa_terms} WHERE aid=%d", $aid->aid)) {
					if (db_query("DELETE FROM {casaa_context_mapper} WHERE aid=%d", $aid->aid)) {
						drupal_set_message('Deleted');
					}
				}
			}
		}
		else if ($form_state['values']['casaa_delete_value']['type'] == 'path') {
			
			$query = db_query("SELECT c.aid FROM {casaa_context_mapper} c 
				LEFT JOIN {casaa_paths} p ON c.aid = p.aid
				WHERE p.casaa_path = (
					SELECT casaa_path FROM {casaa_paths} WHERE aid=%d
				)
				", $form_state['values']['casaa_delete_value']['id']);
				
			while ($aid = db_fetch_object($query)) {
				if (db_query("DELETE FROM	{casaa_paths} WHERE aid=%d", $aid->aid)) {
					if (db_query("DELETE FROM {casaa_context_mapper} WHERE aid=%d", $aid->aid)) {
						drupal_set_message('Deleted');
					}
				}
			}
		}
		//drupal_set_message('Deleting...');
		$commands[] = ctools_modal_command_dismiss(t('Deleted'));
	}
	else if ($form_state['values']['op'] == 'Cancel') {
		drupal_set_message('Canceling...');
		$commands[] = ctools_modal_command_dismiss(t('Canceling'));
	}
	ctools_ajax_render($commands);
}