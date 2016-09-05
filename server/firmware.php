<?php

    $myfile = fopen("version.json", "r") or die("Unable to open file!");
    $result =  fread($myfile,filesize("version.json"));
    fclose($myfile);
    $json = json_decode($result);
    $latestTag = $json->tag_name;
    //echo $latestTag;
                          
    $currentTag = $_GET["tag"];
     if ($latestTag != $currentTag) {
         $download_file = $json->download_file;
        // echo $download_file;

         $myfile = fopen($download_file, "r") or die("Unable to open file!");
         $out =  fread($myfile,filesize($download_file));
         fclose($myfile);
         // Set header for binary
         header('Content-type: application/octet-stream');
         header('Content-disposition: attachment; filename="'.$download_file.'"');
         header('Content-Transfer-Encoding: binary');
         header("Content-Length: ".strlen($out));
         echo $out;
        //echo "nNew";
     } else {
        header($_SERVER["SERVER_PROTOCOL"].' 304 Not Modified', true, 304);
     }
     exit();
?>