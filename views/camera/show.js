 var intervalId = null;
 var counter = null;
 var refresh = null;
 
 jQuery(document).ready(function() {
    var id = location.search.split('id=')[1];
    var duration = parseInt(location.search.split('duration=')[1]);
    refresh = parseInt(location.search.split('refresh=')[1]);

    if (isNaN(refresh)) {
        refresh = 1;
    }

    if (isNaN(duration)) {
        duration = 60;
    }
    
    loadImage(id);
    start(duration, refresh);
});

function loadImage(id) {
    $('#camera').html('<img src="/camera/image/?id=' + id + '&u=1" style="max-height:97vh; min-width:50px; min-height:50px"></a>');
}

function refreshImage() {
    counter--;
    
    if(counter == 0) {
        stop();
    } else {	
        $('#camera img').attr('src', function (i, old) { return old.replace(/&u.+/, "&u=" + (Math.random() * 1000)); });

        if (refresh > 5) {
            setTimeout(function(){ 
                $('#camera img').attr('src', function (i, old) { return old.replace(/&u.+/, "&u=" + (Math.random() * 1000)); });
            }, 1000);
        }
    }
}

function start(time, refresh) {
    counter = time / refresh;
    
    if (intervalId != null) {
        clearInterval(intervalId);
    }
    
    refreshImage();
    intervalId = setInterval(refreshImage, refresh * 1000);
}

function stop() {
  clearInterval(intervalId);
  intervalId = null;
  $('#camera').html('<button onClick="window.location.reload();">Refresh</button>');
}