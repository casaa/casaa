<?php
/**********************************************
 * @file: casaa.add.inc
 * @version: 6.x.2.x-dev
 * @author(s): geoff.maxey / tobby.hagler
 * 
 * Creates and manages the multistep form for 
 * creating new mapping settings within the
 * framework
 ***********************************************/

function casaa_add_settings_callback($js = FALSE) {
	
	//@todo: sanitize!!!!!!!
	$domain = arg(6);
	$type   = arg(7);
	
	$map_obj = new StdClass;
	$map_obj->domain = $domain;
	$map_obj->type   = $type;
	
	$form_state = array('map_obj' => $map_obj);
	
	$step = arg(8);
	if (!$step) {
		$step = 'mapping';
	}
	
	// change the order and forms settings pending what
	// type of mapping we're adding.
	if ($type == 'paths') {
		$order = array(
			'mapping' => t('Create Map'),
			'settings' => t('Add Settings to Mapping'),
		);
		$forms = array(
			'mapping' => array(
				'form id' => 'casaa_mapping_form',
			),
			'settings' => array(
				'form id' => 'casaa_settings_form',
			),
		);
	}
	else if ($type == 'terms') {
		$order = array(
			'mapping' => t('Select Vocabulary'),
			'mapping2' => t('Select Term'),
			'settings' => t('Add Settings To Mapping'),
		);
		$forms = array(
			'mapping' => array(
				'form id' => 'casaa_mapping_form',
			),
			'mapping2' => array(
				'form id' => 'casaa_mapping2_form',
			),
			'settings' => array(
				'form id' => 'casaa_settings_form',
			),
		);
	}
	
	$form_info = array(
		'id' => 'animals',
		'path' => 'admin/build/casaa/mappings/nojs/add/' . $domain . '/' . $type . '/%step',
		'show trail' => TRUE,
		'show back' => TRUE,
		'show cancel' => TRUE,
		'next callback' => 'casaa_wizard_next',
		'finish callback' => 'casaa_wizard_finish',
		'cancel callback' => 'casaa_wizard_cancel',
		'order' => $order,
		'forms' => $forms,
		'ajax' => TRUE,
		'modal' => TRUE,
	);
	
	ctools_include('wizard');
	ctools_include('object-cache');
	
	//INSERT HERE!!!
	// Each pass-through get the cache object and update
	// our form_state object.
	
	$output = ctools_wizard_multistep_form($form_info, $step, $form_state);
	return $output;
}


/** ------Forms and Handlers ---------------------- **/

/** -----Step 1 -------------------**/
/**
 * First Step in the form. 
 */ 
function casaa_mapping_form(&$form, &$form_state) {			
	if ($form_state['map_obj']->type == "paths") {
		$form['casaa_path_value'] = array(
			'#type' => 'textfield',
			'#title' => t('URL Path'),
			'#description' => t('Enter the path you wish to map. Do not include a leading slash. You may use [front] and [404] for the front page and 404\'s. You may also use * as a wilcard.'),
		);
	}
	else if ($form_state['map_obj']->type == "terms") {
		$form['casaa_vocab_values'] = array(
			'#type' => 'select',
			'#title' => t('Vocabularies'),
			'#description' => t('Select the vocabulary you wish to use.'),
			'#options' => _casaa_get_vocabulary_list_options(),
			'#default_value' => variable_get('vocabulary_select', 0),
		);
	}
}

function casaa_mapping_form_submit(&$form, &$form_state) {
	$map = $form_state['map_obj'];
	$values = $form_state['values'];
	if ($map->type == "terms") {
		variable_set('vocabulary_select', $values['casaa_vocab_values']);
		$form_state['map_obj']->vocab = $values['casaa_vocab_values'];
	}
	else if ($map->type == 'paths') {
		$form_state['map_obj']->path = $values['casaa_path_value'];
	}
}

/** ----- Optional Step 2 ---------- **/

function casaa_mapping2_form(&$form, &$form_state) {
	$form['casaa_term_values'] = array(
		'#type' => 'select',
		'#title' => t('Taxonomy Term'),
		'#description' => t('Select the vocabulary you wish to use.'),
		'#options' => _casaa_get_taxonomy_list_options(),
		'#prefix' => '<div class="clear-block" id="casaa-term-select">',
		'#suffix' => '</div>',
	);
}

function casaa_mapping2_form_submit(&$form, &$form_state) {
	$values = $form_state['values'];
	$form_state['map_obj']->term = $values['casaa_term_values'];
}


/** ------ Final Step ----------------- **/

function casaa_settings_form(&$form, &$form_state) {
	$plugins = _casaa_load_plugins();
	//var_dump($plugins);
	$form['casaa_settings_form'] = array(
		'#type' => 'textfield',
		'#title' => 'second step',
	);
}

function casaa_settings_form_submit(&$form, &$form_state) {
	$values = $form_state['values'];
	$form_state['map_obj']->values = $values;
}


/** ----- Caching and Control Callbacks -------------**/

/**
 * Callback for the next step
 * 
 * Grab our data from the form state and set the cache object
 */ 
function casaa_wizard_next(&$form_state) {
	$map = &$form_state['map_obj'];
	$cache = ctools_object_cache_set('casaa_mapping', $form_state['cache name'], $map);
}


/**
 * Save our form data and clear the form cache object.
 */ 
function casaa_wizard_finish(&$form_state) {
	$map = $form_state['map_obj'];
	if (casaa_mapping_save($map)) {
		drupal_set_message('Mapping has been created');
	}
	else {
		drupal_set_message('Unable to save!', 'error');
	}
	ctools_object_cache_clear('casaa_mapping', $form_state['cache name']);
	$form_state['redirect'] = 'admin/build/casaa/main/' . $map->domain;
}

/**
 * Callback for the cancel action.
 * 
 * Clear the cache object and redirect the user.
 */ 
function casaa_wizard_cancel(&$form_state) {
	$map = $form_state['map_obj'];
	ctools_object_cache_clear('casaa_mapping', $form_state['cache name']);
	$form_state['redirect'] = 'admin/build/casaa/main/' . $map->domain;
	drupal_set_message('Action Cancelled');
}


/** ------- Mapping Settings Save Functions ------------**/

/**
 * Save the settings from the form
 */
function casaa_mapping_save($values = NULL) {
	var_dump($values);die;
}



/** ---- Taxonomy Options Functions -------------------**/

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
function _casaa_get_taxonomy_list_options() {
	$vocab_option = variable_get('vocabulary_select', 0);
	$vocab_options = _casaa_get_vocabulary_list_options();
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

function _casaa_get_vocabulary_list_options() {
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