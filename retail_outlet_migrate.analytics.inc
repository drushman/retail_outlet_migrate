<?php

function table_opening_hours($s) {
  $s = preg_replace('/\s+/', '', $s);
  $s = str_replace ('to', '-', $s);
  $s = strtolower($s);

  $n = strlen($s);
  $rows = array();
  $i = 0;
  $day = "";
  $start_time = "";
  $end_time = "";
  $index = 0;
  while ($i < $n){
    $i = get_day($i,$day,$n,$s);
    $i = get_start_time($i,$start_time,$n,$s);
    $i = get_end_time($i,$end_time,$n,$s);
    $rows[$index]['day'] = $day;
    $rows[$index]['start_time'] = $start_time;
    $rows[$index]['end_time'] = $end_time;
    $day = "";
    $start_time = "";
    $end_time = "";
    ++$index;
  }
  $result = analytics_day($rows);
  ksort($result);
  return $result;
}



function get_day($i, &$day,$n,$s) {
  for ($i; $i < $n; $i++) {
    $number = number_c($s[$i]);
    if ($number){
      return $i;
    }
    $day .= $s[$i];
  }
  return $i;
}
function get_start_time($i, &$start_time,$n,$s) {
  $not = FALSE;
  for ($i; $i < $n; $i++) {
    $number = number_c($s[$i]);
    if (!$number && !$not){
      if ($s[$i] == ':' && number_c($s[$i+1])) {
        $start_time .= $s[$i];
        continue;
      }
      if ($s[$i] == '-' && number_c($s[$i+1])) {
        //$end_time .= $s[$i];
        ++$i;
        return $i;
      }
      if ($s[$i + 1] == 'm') {
        $start_time .= $s[$i];
        $start_time .= $s[$i+1];
        $i = $i + 1;
        $not = TRUE;
        continue;
      }
      
      else {
        return $i;
      }
      //return $i;
    }
    if ($number && $not){
      return $i;
    }
    if (!$number && $not){
      continue;
    }
    
    $start_time .= $s[$i];
  }
  return $i;
}
function get_end_time($i, &$end_time,$n,$s) {
  $not = FALSE;
  for ($i; $i < $n; $i++) {
    $number = number_c($s[$i]);
    if (!$number && !$not) {
      if ($s[$i] == ':' && number_c($s[$i+1])) {
        $end_time .= $s[$i];
        continue;
      }
      if ($s[$i + 1] == 'm') {
        $end_time .= $s[$i];
        $end_time .= $s[$i+1];
//        print_r( 'i--' .$s[$i] . '--i|');
//        print_r( 'i+1--' .$s[$i+1] . '--i+1|');
//        print_r( 'i+2--' .$s[$i+2] . '--i+2|');
        $i += 1;
        $not = TRUE;
        continue;
      }
      else {
        return $i;
      }
    }
    if (!$number && $not){
      return $i;
    }
    $end_time .= $s[$i];
  }
  return $i;
}

function number_c($c) {
  $str = "0123456789";
  $pos = strpos($str,$c);

  if($pos === false) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

function string_find_string($find, $str) {
  //$str = "0123456789";
  $pos = strpos($str,$find);

  if($pos === false) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

function analytics_day($day_times) {
  $results = array();
  foreach ($day_times as $day_time){
    $day_time['day'] = clear_day($day_time['day']);
    $result = analytics_day_by_day($day_time);
    if (!isset($result['1'])) {
      continue;
    }
    foreach ($result['1'] as $key => $value) {
      $results[$key] = $value;
    }
  }
  if(!isset($results)) {
    return $results;
  }
  make_ensure_format($results);
  return $results;
  
}
function make_ensure_format(&$result) {
  $day_null["cell_0_"] = "";
  $day_null["cell_1_"] = "Monday";
  $day_null["cell_2_"] = "Tuesday";
  $day_null["cell_3_"] = "Wednesday";
  $day_null["cell_4_"] = "Thursday";
  $day_null["cell_5_"] = "Friday";
  $day_null["cell_6_"] = "Saturday";
  $day_null["cell_7_"] = "Sunday";
  
  $key["cell_0_0"] = $result["cell_0_1"] = $result["cell_0_2"] = '';
  foreach ($day_null as $key => $value) {
    if (isset($result[$key . "0"])) {
      continue;;
    }
    $result[$key . "0"] = $value;
    $result[$key . "1"] = "";
    $result[$key . "2"] = "";
  }
  $result["rebuild"]["count_cols"] = 3;
  $result["rebuild"]["count_rows"] = 8;
  $result["rebuild"]["rebuild"]    = "Rebuild Table";
  
  $result["import"]["tablefield_csv_field_opening_hours_0"] = "";
  $result["rebuild"]["rebuild_field_opening_hours_0"] = "Upload CSV";
   
}

function analytics_day_by_day($days) {
  //print_r($days['day']);
  if (string_find_string('-', $days['day'])) {
    $days_slipt = explode("-", $days['day']);
    $dayf = $days_slipt['0'];
    $dayt = $days_slipt['1'];
    $result = analytics_day_by_($dayf, $dayt, $days['start_time'], $days['end_time']);
    
    //print_r($dayf);
    //print_r($dayt);
  }
  if (string_find_string(',', $days['day'])) {
    $days_slipt = explode(",", $days['day']);
    $dayf = $days_slipt['0'];
    $dayt = $days_slipt['1'];
    $result = analytics_day_by_comma($dayf, $dayt, $days['start_time'], $days['end_time']);
    
    //print_r($dayf);
    //print_r($dayt);
  }
  $not = string_find_string('-', $days['day']);
  $not1 = string_find_string(',', $days['day']);
  if (!$not && !$not1 ) {
    $result = analytics_day_by_one($days['day'], $days['start_time'], $days['end_time']);
  }
  return $result;
}

function clear_day($str){
  $chars = array(",", ":", "-");
  $result = $str;
  if (in_array($result['0'], $chars)) {
    $result = substr($result, 1);
  }
  if (in_array($result[strlen($result)-1], $chars)) {
    $result = substr($result, 0, strlen($result)-1);
  }
  return $result;
}


function analytics_day_by_($dayf, $dayt, $start_time, $end_time) {
  
  $results = array();
  $index = 1;
  $start_push = FALSE;
  $day_short['2'] = array("m", "mon", "monday", "Monday");
  $day_short['3'] = array("tu", "tues", "tuesday", "Tuesday");
  $day_short['4'] = array("w", "wed", "wednesday", "Wednesday");
  $day_short['5'] = array("t", "thurs", "thursday", "Thursday", "thu");
  $day_short['6'] = array("f", "fri", "friday", "Friday");
  $day_short['7'] = array("s", "sat", "saturday", "Saturday");
  $day_short['8'] = array("s", "sun", "sunday", "Sunday");
  
  foreach ($day_short as $key => $values) {
    if ($key == 8){
      $key = 1;
    }
    if ($start_push) {
      if (in_array($dayt, $values)) {
        $result["cell_" . $key . "_0"] = $values['3'];
        $result["cell_" . $key . "_1"] = $start_time;
        $result["cell_" . $key . "_2"] = $end_time;
        $results[$index] = $result;
        break;
      }
      $result["cell_" . $key . "_0"] = $values['3'];
      $result["cell_" . $key . "_1"] = $start_time;
      $result["cell_" . $key . "_2"] = $end_time;
      $results[$index] = $result;
    }
    
    if (in_array($dayf, $values) && !$start_push) {
      $result["cell_" . $key . "_0"] = $values['3'];
      $result["cell_" . $key . "_1"] = $start_time;
      $result["cell_" . $key . "_2"] = $end_time;
      $results[$index] = $result;
      $start_push = TRUE;
    }
    
  }
//  print_r('____'); 
//  print_r($results); 
  return $results;
}

function analytics_day_by_comma($days, $start_time, $end_time){
//  $days= array(
//    "mon",
//    "tues",
//    "thurs",
//  );
//  $start_time = "6pm";
//  $end_time = "8pm";
  $results = array();
  $index = 1;
  $day_short['2'] = array("m", "mon", "monday", "Monday");
  $day_short['3'] = array("tu", "tues", "tuesday", "Tuesday");
  $day_short['4'] = array("w", "wed", "wednesday", "Wednesday");
  $day_short['5'] = array("t", "thurs", "thursday", "Thursday", "thu");
  $day_short['6'] = array("f", "fri", "friday", "Friday");
  $day_short['7'] = array("s", "sat", "saturday", "Saturday");
  $day_short['8'] = array("s", "sun", "sunday", "Sunday");
  foreach ($days as $day) {
    foreach ($day_short as $key => $values) {
      if ($key == 8){
        $key = 1;
      }
      if (in_array($day, $values)) {
        $result["cell_" . $key . "_0"] = $values['3'];
        $result["cell_" . $key . "_1"] = $start_time;
        $result["cell_" . $key . "_2"] = $end_time;
        $results[$index] = $result;
        //++$index;
        break;
      }
    }
  }
//  print_r(',,,,,,,'); 
  //print_r($results);    
  return $results;
}

function analytics_day_by_one($days, $start_time, $end_time){
//  $days= array(
//    "mon",
//    "tues",
//    "thurs",
//  );
//  $start_time = "6pm";
//  $end_time = "8pm";
  $results = array();
  $index = 1;
  $day_short['2'] = array("m", "mon", "monday", "Monday");
  $day_short['3'] = array("tu", "tues", "tuesday", "Tuesday");
  $day_short['4'] = array("w", "wed", "wednesday", "Wednesday");
  $day_short['5'] = array("t", "thurs", "thursday", "Thursday", "thu");
  $day_short['6'] = array("f", "fri", "friday", "Friday");
  $day_short['7'] = array("s", "sat", "saturday", "Saturday");
  $day_short['8'] = array("s", "sun", "sunday", "Sunday");
  foreach ($day_short as $key => $values) {
    if ($key == 8){
      $key = 1;
    }
    if (in_array($days, $values)) {
      $result["cell_" . $key . "_0"] = $values['3'];
      $result["cell_" . $key . "_1"] = $start_time;
      $result["cell_" . $key . "_2"] = $end_time;
      $results[$index] = $result;
//      print_r('ooooo'); 
//      print_r($results);    
      return $results;
    }
  }
  return $results;
  
}