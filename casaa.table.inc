<?php

/**
 * Build the modal window to display the table.
 * */
function casaa_build_tables_callback($js = FALSE) {
	$title = t('Table View');
	$output = "Hello modal window!";
	
	if ($js) {
    ctools_include('ajax');
    ctools_include('modal');
    ctools_modal_render($title, $output);
    // above command will exit().
  }
  else {
    drupal_set_title($title);
    return $output;
  }
}