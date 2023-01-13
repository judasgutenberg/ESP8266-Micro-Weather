<?php 
function gvfw($name, $fail = false){ //get value from wherever
  if(isset($_REQUEST[$name])) {
    return $_REQUEST[$name];
  }
  return $fail;
}
 
