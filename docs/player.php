<?php
$enlace =  $_GET["media"] ;

$pagina_inicio = file_get_contents('http://www.mediafire.com/file/' . $enlace. '/');
$start = strpos($pagina_inicio, 'http://download');
$end = strpos($pagina_inicio, "'",$start);

//echo "$start \n $end \n"  ;
    $pagina_inicio = substr($pagina_inicio, $start,$end - $start);
//echo $pagina_inicio;

?>
<head>
  <link href="http://vjs.zencdn.net/6.2.8/video-js.css" rel="stylesheet">

  <!-- If you'd like to support IE8 -->
  <script src="http://vjs.zencdn.net/ie8/1.1.2/videojs-ie8.min.js"></script>
  <style>
body {
    background-color: #000000;
}
</style>
</head>

<body>

<center>
  <video id="my-video" class="video-js" controls preload="auto" width="720" height="480"
  poster="MY_VIDEO_POSTER.jpg" data-setup="{}">
    <source src="<?php echo $pagina_inicio;  ?>" type='video/mp4'>
    <p class="vjs-no-js">
      To view this video please enable JavaScript, and consider upgrading to a web browser that
      <a href="http://videojs.com/html5-video-support/" target="_blank">supports HTML5 video</a>
    </p>
  </video>

  <script src="http://vjs.zencdn.net/6.2.8/video.js"></script>
  </center>
</body>