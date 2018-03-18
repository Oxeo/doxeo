<script>
var updateAll = true;
var lastLogId = 0;
var listCmd = [];
var listCmdPosition = -1;

jQuery(document).ready(function() {
    update();
    updateCmdList();
    
    $("#searchInput").on("keyup", function() {
      var value = $(this).val().toLowerCase();
      $("#logsTable tr").filter(function() {
        $(this).toggle($(this).text().toLowerCase().indexOf(value) > -1)
      });
    });
});

function update() {
    if (!updateAll) {
        return;
    }
    
    $.getJSON('logs.js?log=debug&startid=' + lastLogId).done(function(result) {
        if (result.success) {
            if (lastLogId == 0) {
                $('#logsTable').html("");
                setInterval(update, 1000);
            }
            
            $.each(result.messages, function(key, val) {
                if (val.type === "critical") {
                    $('#logsTable').prepend('<tr class="danger"><td>'+val.date+'</td><td>'+val.message+'</td></tr>')
                } else if (val.type === "warning") {
                    $('#logsTable').prepend('<tr class="warning"><td>'+val.date+'</td><td>'+val.message+'</td></tr>')
                } else {
                    var msg = val.message.replace("uFFFD\\uFFFD\\uFFFD\\uFFFD\\uFFFD\\uFFFD\\uFFFD\\uFFFD\\uFFFD\\uFFFD\\uFFFD\\uFFFD\\uFFFD\\uFFFD\\", "...")
                    $('#logsTable').prepend('<tr><td>'+val.date+'</td><td>'+msg+'</td></tr>')
                }
                lastLogId = val.id + 1;
            });
            
            // filter
            var value = $("#searchInput").val().toLowerCase();
            $("#logsTable tr").filter(function() {
                $(this).toggle($(this).text().toLowerCase().indexOf(value) > -1)
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

function updateCmdList() {
    $.getJSON('/script/cmd_list.js').done(function(result) {
        listCmd = [];
        $('#cmdlist').empty();
        var regex = new RegExp('"', 'g');
        
        result.records.sort(function(a, b) {
            return Number(b.id) - Number(a.id);
        })
        
        $.each(result.records, function(key, val) {
            listCmd.push(val.cmd);
            $('#cmdlist').append('<option value="' + val.cmd.replace(regex, '&quot;') + '" />');
        });
        
        listCmdPosition = -1;
    }).fail(function(jqxhr, textStatus, error) {
        alert_error("Request Failed: " + error);
    });
}

$('#clear_logs').click(function(event){
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

$('#stop_logs').click(function(event){
	if (updateAll == true) {
		updateAll = false;
		$('#stop_logs').text("Continue");
	} else {
		updateAll = true;
		$('#stop_logs').text("Stop");
	}
});

$('#send').click(function(event) {
    var cmd = $('#sendContent').val();
    sendCmd(cmd);
});

$('#sendContent').on('keyup', function (e) {
    if(e.which === 13) { // enter key
        var cmd = $('#sendContent').val();
        sendCmd(cmd);
    } /*else if (e.which === 38) { // top key
        if (listCmdPosition+1 < listCmd.length) {
            listCmdPosition++;
            $('#sendContent').val(listCmd[listCmdPosition]);
         }
    } else if (e.which === 40) { // down key
         if (listCmdPosition >= 0) {
            listCmdPosition--;
            if (listCmdPosition == -1) {
                $('#sendContent').val("");
            } else {
                $('#sendContent').val(listCmd[listCmdPosition]);
            }
         }
    }*/
});



$('#buildBoardCmd').click(function(event){
    var cmd = $('#sendContent').val();
    $('#sendContent').val('helper.sendCmd("' + cmd + '")');
});

function sendCmd(cmdToSend) {
    var param = {
        cmd: cmdToSend
    };
    
    $.getJSON('/script/execute_cmd.js', param)
        .done(function(result) {
            if (result.success) {
                $('#sendContent').val(result.msg);
                updateCmdList();
            } else {
                alert_error(result.msg);
            }
        }).fail(function(jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
    });
}

$('#clearSearch').click(function(event){
    $('#searchInput').val("");
});

$('#clearCmdContent').click(function(event){
    $('#sendContent').val("");
    listCmdPosition = -1;
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
