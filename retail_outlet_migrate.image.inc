<?php
// Convert Image filename
function convert_to_filename ($string) {

	$string = strtolower($string);
	$string = str_replace (" ", "_", $string);
	$string = str_replace ("'", "", $string);
	$string = str_replace (",", "", $string);
	$string = str_replace (".", "", $string);

	preg_replace ("/[^0-9^a-z^_^.]/", "", $string);
	if($string=='') return 'default_photo_name';
	return $string;
}

// Write image to drupal site
function write_image_info($remote_image_path, $remote_image_name ,$remote_image_extension , &$fid) {

  if (empty($remote_image_extension)) {
    $remote_image_extension = "jpg";
  }
	// Save image file to drupal
	$remote_image_name = convert_to_filename($remote_image_name);

	$file_temp = file_get_contents($remote_image_path);
  $file_save_path  = file_default_scheme() . "://store-pictures/";
  $file_save_path .= $remote_image_name . "." . $remote_image_extension;
	$file = file_save_data($file_temp, $file_save_path, FILE_EXISTS_RENAME);

  $fid = $file->fid;
}


