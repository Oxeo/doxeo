<script>
var updateError = false;

jQuery(document).ready(function() {
    update();
    setInterval(update, 1000);
});

function update() {
    if (updateError) {
        return;
    }
    
    $.getJSON('logs.js?log=debug').done(function(result) {
        if (result.success) {
            if (result.debug.length > 0) {
                $('#clearButton_placeholder').html('<button type="button" class="btn btn-primary clear_logs" data-clear="debug">Clear</button>');
            } else {
                $('#clearButton_placeholder').html("");
            }
            
            $('#logsTable').html("");
            $.each(result.debug, function(key, val) {
                $('#logsTable').append('<tr><td>'+val.date+'</td><td>'+val.message+'</td><td>'+val.file.split("/").pop()+' ('+val.line+')</td></tr>')
            });
        } else {
            updateError = true;
            alert_error(result.msg);
        }
    }).fail(function(jqxhr, textStatus, error) {
        updateError = true;
        alert_error("Request Failed: " + error);
    });
}

$('#clearButton_placeholder').on('click', '.clear_logs', function(){
    var data = $(this).data();
    
    param = {
        log: data.clear
    };
    
    $.getJSON('clear_logs.js', param)
        .done(function(result) {
            if (result.success) {
                history.back();
            } else {
                alert_error(result.msg);
            }
        }).fail(function(jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
    });
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
