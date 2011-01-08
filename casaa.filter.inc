<?php

/*********************************************
 * @file: casaa.fitler.inc
 * @author: cntlscrut
 * @version: 6.x-1.12-rc1
 * 
 * Provides a filter form for sorting and searching for values.
 *********************************************/

/**
 * Build the filtering form. 
 **/
function casaa_filter_form() {
	$form['casaa_filter_form'] = array(
		'#type' => 'fieldset',
		'#title' => t('Filter/Search'),
		'#collapsible' => TRUE,
	);
		$form['casaa_filter_form']['casaa_type'] = array(
			'#type' => 'select',
			'#title' => t('Mapping Type Is:'),
			'#options' => array('all' => t('All'), 'term' => t('Term Mapping'), 'path' => t('Path Mapping')),
			'#default_value' => 'all',
		);
		$form['casaa_filter_form']['casaa_search_field'] = array(
			'#type' => 'textfield',
			'#title' => t('Search'),
			'#description' => t('Search for the name of a term or a url path.'),
		);
		$form['casaa_filter_form']['casaa_filter_submit'] = array(
			'#type' => 'submit',
			'#value' => t('Search'),
		);
	
	return $form;
}

/**
 * Take the search terms from the user and go to the filter/search page
 * with the proper arguments. 
 **/
function casaa_filter_form_submit($form, &$form_state) {
	if ($form_state['values']['op'] == "Search") {
		$path = 'admin/build/casaa/filter';
		$query_vars = array('type' => $form_state['values']['casaa_type'], 'search' => urlencode(str_replace(' ', "+", $form_state['values']['casaa_search_field'])));
		
		drupal_goto($path, $query_vars);
	}
}


/**
 * Build the filter/search page 
 **/
function casaa_filter_page() {
	$type = filter_xss($_GET['type']);
	$search = filter_xss($_GET['search']);
	$search = str_replace('+', ' ', urldecode($search));
	
	$output = '';
	$output .= drupal_get_form('casaa_filter_form');
	
	$header = array(t('Mapping Type'), t('Name/Path'), t('Options'));
	
	if ($type == 'term' || $type == 'all') {
		$query = db_query("SELECT DISTINCT t.name FROM {casaa_terms} c LEFT JOIN {term_data} t ON c.casaa_tid=t.tid WHERE t.name LIKE '%s%'", $search);
		while ($terms = db_fetch_object($query)) {
			$col = '';
			$col[] = t('Term');
			$col[] = $terms->name;
			$aid_query = db_query("SELECT c.casaa_tid FROM {casaa_terms} c LEFT JOIN {term_data} t ON c.casaa_tid=t.tid WHERE t.name LIKE '%s'", $terms->name);
			$result = db_fetch_object($aid_query);
			$col[] = l('configure', 'admin/casaa/edit/term/' . $result->casaa_tid) . ' / ' . l('delete', 'admin/casaa/delete/term/' . $result->casaa_tid);
			$rows[] = $col;
		}
	}
	if ($type == 'all' || $type == 'path') {
		if ($search == "front" || $search == 'frontpage') {
			$search = "[front]";
		}
		else if ($search == '404') {
			$search = "[404]";
		}
		$query = db_query("SELECT DISTINCT casaa_path FROM {casaa_paths} WHERE casaa_path LIKE '%s%'", $search);
		while ($path = db_fetch_object($query)) {
			$col = '';
			$col[] = t('Path');
			$col[] = $path->casaa_path;
			$aid_query = db_query("SELECT aid FROM {casaa_paths} WHERE casaa_path='%s'", $path->casaa_path);
			$result = db_fetch_object($aid_query);
			$col[] = l('configure', 'admin/casaa/edit/path/' . $result->aid) . ' / ' . l('delete', 'admin/casaa/delete/path/' . $result->aid);
			$rows[] = $col;
		}
	}
	if (empty($rows) && $search) {
		$output .= "<strong>No results were found for \"" . $search . "\"</strong>";
	}
	else if (!empty($rows)) {
		$output .= theme_table($header, $rows);
	}
	return $output;
}








