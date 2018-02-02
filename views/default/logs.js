<script>
var updateAll = true;

jQuery(document).ready(function() {
    update();
    setInterval(update, 1000);
});

function update() {
    if (!updateAll) {
        return;
    }
    
    $.getJSON('logs.js?log=debug').done(function(result) {
        if (result.success) {            
            $('#logsTable').html("");
            $.each(result.messages, function(key, val) {
                if (val.type === "critical") {
                    $('#logsTable').prepend('<tr class="danger"><td>'+val.date+'</td><td>'+val.message+'</td></tr>')
                } else if (val.type === "warning") {
                    $('#logsTable').prepend('<tr class="warning"><td>'+val.date+'</td><td>'+val.message+'</td></tr>')
                } else {
                    $('#logsTable').prepend('<tr><td>'+val.date+'</td><td>'+val.message+'</td></tr>')
                }
            });
        } else {
            updateAll = false;
            alert_error(result.msg);
        }
    }).fail(function(jqxhr, textStatus, error) {
        updateAll = false;
        alert_error("Request Failed: " + error);
    });
}

$('#buttons_placeholder').on('click', '.clear_logs', function(){
    var data = $(this).data();
    
    param = {
        type: data.clear,
        id: 0
    };
	
	$('#logsTable').text("");
    
    $.getJSON('clear_logs.js', param)
        .done(function(result) {
            if (result.success) {
                //history.back();
            } else {
                alert_error(result.msg);
            }
        }).fail(function(jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
    });
});

$('#buttons_placeholder').on('click', '.stop_logs', function(){
	if (updateAll == true) {
		updateAll = false;
		$('.stop_logs').text("Continue");
	} else {
		updateAll = true;
		$('.stop_logs').text("Stop");
	}
});

function alert_error(message) {           
    $('#alert_placeholder').prepend(
        '<div style="display: none;" class="alert alert-danger fade in" role="alert">' +
            '<strong>Error!</strong> '+message+
            '<button type="button" class="close" data-dismiss="alert" aria-label="Close">' +
                '<span aria-hidden="true">&times;</span>' +
            '</button>'+
        '</div>'
    );
    
    $('.alert-danger').slideDown( "fast" );
}

</script>
