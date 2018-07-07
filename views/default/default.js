<script src="../assets/bootstrap-switch/bootstrap-switch.min.js" type="text/javascript"></script>

<script>
var updateError = false;

jQuery(document).ready(function() {
    update();
    setInterval(update, 30000);
});

function update() {
    if (updateError) {
        return;
    }
    
    $.getJSON('system.js').done(function(result) {
        if (result.success) {
            $('#system_time').html(result.time);
            if (result.device_connected) {
                $('#board_status').html('<span class="label label-info">Connected</span>');
            } else {
                $('#board_status').html('<span class="label label-danger">Disconnected</span>');
            }
        } else {
            updateError = true;
            alert_error(result.msg);
        }
    }).fail(function(jqxhr, textStatus, error) {
        updateError = true;
        alert_error("Request Failed: " + error);
    });
    
    $.getJSON('switch/switch_list.js').done(function(result) {
        if (result.Result == "OK") {
            $('#switchList').html('');
            result.Records.sort(function(a, b) { 
                return a.order - b.order;
            })
            $.each(result.Records, function(key, val) {
                $('#switchList').append('<tr><td class="text-right" style="width: 50%">'+val.name+'</td><td class="text-left"><input id="sw_'+val.id+'" name="switch" class="switch_on_off" type="checkbox" data-id="'+val.id+'" data-size="mini"></td></tr>');
                if (val.status == "on" || val.status == "pending") {
                    $('#sw_'+val.id+'').prop('checked', true);
                }
            });
            $("[name='switch']").bootstrapSwitch();
        } else {
            $('#switchList').html('<tr><td></td></tr>');
            updateError = true;
            alert_error(result.msg);
        }
    }).fail(function(jqxhr, textStatus, error) {
        $('#switchList').html('<tr><td></td></tr>')
        updateError = true;
        alert_error("Request Failed: " + error);
    });
    
    $.getJSON('thermostat/heaters.js').done(function(result) {
        if (result.Result == "OK") {
            var create = ($('#heater_pan1').length == 0) ? false : true;
            
            $.each(result.Records, function(key, heater) {
                if (create) {
                    if (key != 0) {
                        $('#heater_nav_tabs li:last-child').clone().insertAfter('#heater_nav_tabs li:last-child');
                        $('#heater_nav_tabs li:last-child').removeClass("active");
                        $('#heater_tab_panes div.tab-pane:last-child').clone().insertAfter('#heater_tab_panes div.tab-pane:last-child');
                        $('#heater_tab_panes div.tab-pane:last-child').removeClass("active");
                    }

                    if (heater.id === 1) {
                      $('#heater_nav_tabs li:last-child').addClass("active");
                      $('#heater_tab_panes div.tab-pane:last-child').addClass("active");
                    }
                    
                    $('#heater_nav_tabs li:last-child').find("a").text(heater.name);
                    $('#heater_nav_tabs li:last-child').find("a").prop('href', '#heater_'+heater.id);
                    $('#heater_nav_tabs li:last-child').find("a").attr('aria-controls', 'heater_'+heater.id);
                    $('#heater_tab_panes div.tab-pane:last-child').attr('id', 'heater_'+heater.id);
                }
                
                $('#heater_'+heater.id).find(".heater_temperature").html(heater.temperature.toFixed(2)+'&deg;');
                $('#heater_'+heater.id).find(".heat_setpoint").text(heater.heat_setpoint.toFixed(1));
                $('#heater_'+heater.id).find(".cool_setpoint").text(heater.cool_setpoint.toFixed(1));
                $('#heater_'+heater.id).find(".heater_mode").text(heater.mode);
                $('#heater_'+heater.id).find(".heater_status").text(heater.status);
                
                if (heater.current_setpoint == heater.heat_setpoint) {
                    $('#heater_'+heater.id).find(".heat_setpoint").css("font-weight","Bold");
                } else {
                    $('#heater_'+heater.id).find(".heat_setpoint").css("font-weight","Normal");
                }
                
                if (heater.current_setpoint == heater.cool_setpoint) {
                    $('#heater_'+heater.id).find(".cool_setpoint").css("font-weight","Bold");
                } else {
                    $('#heater_'+heater.id).find(".cool_setpoint").css("font-weight","Normal");
                }
            });
        } else {
            updateError = true;
            alert_error(result.msg);
        }
    }).fail(function(jqxhr, textStatus, error) {
        updateError = true;
        alert_error("Request Failed: " + error);
    });
    
    $.getJSON('thermostat/status.js').done(function(result) {
        if (result.success) {           
            if (result.thermostat_status === 1) {
                $('#thermostat_status_active').find('span').first().text("Running");
                $('#thermostat_status_active').removeClass('btn-secondary btn-info btn-warning').addClass('btn-info');
            } else if (result.thermostat_status === 2) {
                $('#thermostat_status_active').find('span').first().text("On Break");
                $('#thermostat_status_active').removeClass('btn-secondary btn-info btn-warning').addClass('btn-warning');
            } else {
                $('#thermostat_status_active').find('span').first().text("Stopped");
                $('#thermostat_status_active').removeClass('btn-secondary btn-info btn-warning').addClass('btn-secondary');
            }
            
            if (result.temperaturelogger_enable) {
                $('#temperaturelogger_status_active').find('span').first().text("Running");
                $('#temperaturelogger_status_active').removeClass('btn-secondary btn-info').addClass('btn-info');
            } else {
                $('#temperaturelogger_status_active').find('span').first().text("Stopped");
                $('#temperaturelogger_status_active').removeClass('btn-secondary btn-info').addClass('btn-secondary');
            }
        } else {
            updateError = true;
            alert_error(result.msg);
        }
    }).fail(function(jqxhr, textStatus, error) {
        updateError = true;
        alert_error("Request Failed: " + error);
    });
    
    $.getJSON('logs.js?type=warning').done(function(result) {
        if (result.success) {            
            var cptWarning = 0;
            var cptCritical = 0;
            
            $('#errorModalTable').html("");
            $('#warningModalTable').html("");
            
            $.each(result.messages, function(key, val) {
                if (val.type === "critical") {
                    cptCritical++;
                    $('#errorModalTable').append('<tr><td>'+val.date+'</td><td>'+val.message+'</td></tr>')
                } else if (val.type === "warning") {
                    cptWarning++;
                    $('#warningModalTable').append('<tr><td>'+val.date+'</td><td>'+val.message+'</td></tr>')
                }
            });
        
            if (cptCritical > 0) {
                $('#critical_error').html('<button type="button" class="btn btn-danger btn-xs" data-toggle="modal" data-target="#errorModal">'+cptCritical+'</button>');
            } else {
                $('#critical_error').html('<span class="label label-success">0</span>');
            }
            
            if (cptWarning > 0) {
                $('#warning_error').html('<button type="button" class="btn btn-danger btn-xs" data-toggle="modal" data-target="#warningModal">'+cptWarning+'</button>');
            } else {
                $('#warning_error').html('<span class="label label-success">0</span>');
            }
        } else {
            updateError = true;
            alert_error(result.msg);
        }
    }).fail(function(jqxhr, textStatus, error) {
        updateError = true;
        alert_error("Request Failed: " + error);
    });
	
	$.getJSON('sensor/sensor_list.js').done(function(result) {
        if (result.Result == "OK") {
            $('#sensorList').html('');
            $('#batteryList').html('');
            result.Records.sort(function(a, b) { 
                if (a.name < b.name) {
                    return -1;
                } else if (a.name > b.name) {
                    return 1;
                } else {
                    return 0;
                }
            })
            $.each(result.Records, function(key, val) {
                // update sensor panel
                var date = new Date(val.last_event * 1000);;
                var lastUpdate = date.toLocaleTimeString() + ' ' + date.toLocaleDateString();
                $('#sensorList').append('<tr><td class="text-right" style="width: 50%">'+val.name+'</td><td class="text-left"><span data-toggle="tooltip" data-placement="right" title="'+lastUpdate+'">'+val.value+'</span></td></tr>');
                
                // update battery panel
                if (val.battery > 0) {
                    date = new Date(val.battery_update * 1000);;
                    lastUpdate = date.toLocaleTimeString() + ' ' + date.toLocaleDateString();
                    $('#batteryList').append('<tr><td class="text-right" style="width: 50%">'+val.name+'</td><td class="text-left"><span data-toggle="tooltip" data-placement="right" title="'+lastUpdate+'">'+val.battery+'%</span></td></tr>');
                }
            });
            $('[data-toggle="tooltip"]').tooltip();
        } else {
            $('#sensorList').html('<tr><td></td></tr>');
            $('#batteryList').html('<tr><td></td></tr>');
            updateError = true;
            alert_error(result.msg);
        }
    }).fail(function(jqxhr, textStatus, error) {
        $('#sensorList').html('<tr><td></td></tr>');
        $('#batteryList').html('<tr><td></td></tr>');
        updateError = true;
        alert_error("Request Failed: " + error);
    });
	
	$.getJSON('script/script_list.js').done(function(result) {
        if (result.Result == "OK") {
            $('#scriptList').html('');
            result.Records.sort(function(a, b) { 
                if (a.name < b.name) {
                    return -1;
                } else if (a.name > b.name) {
                    return 1;
                } else {
                    return 0;
                }
            })
            $.each(result.Records, function(key, val) {
				status = "\
				<div class=\"dropdown\"> \
				  <button class=\"btn btn-xs dropdown-toggle\" type=\"button\" style=\"border: 0px; padding:0px 5px 1px 5px; font-size:11px;\" id=\"script_status_"+val.id+"\" data-toggle=\"dropdown\" aria-haspopup=\"true\" aria-expanded=\"false\"> \
					<span></span> <span class=\"caret\"></span> \
				  </button> \
				  <ul class=\"dropdown-menu\" aria-labelledby=\"dropdownMenuMode\"> \
					<li><a href='javascript:void(0)' class='change_script_status' data-id='"+val.id+"' data-status='on'>Enable</a></li> \
					<li><a href='javascript:void(0)' class='change_script_status' data-id='"+val.id+"' data-status='off'>Disable</a></li> \
				  </ul> \
				</div>";
				
				$('#scriptList').append('<tr><td class="text-right" style="width: 50%">'+val.name+'</td><td class="text-left">'+status+'</td></tr>');
				 
				if (val.status === "on") {
					$('#script_status_'+val.id).find('span').first().text("Enabled");
					$('#script_status_'+val.id).removeClass('btn-secondary btn-info').addClass('btn-info');
				} else {
					$('#script_status_'+val.id).find('span').first().text("Disabled");
					$('#script_status_'+val.id).removeClass('btn-secondary btn-info').addClass('btn-secondary');
				}
				 
            });
        } else {
            $('#scriptList').html('<tr><td></td></tr>');
            updateError = true;
            alert_error(result.msg);
        }
    }).fail(function(jqxhr, textStatus, error) {
        $('#scriptList').html('<tr><td></td></tr>')
        updateError = true;
        alert_error("Request Failed: " + error);
    });
}

$('#switchList').on('switchChange.bootstrapSwitch', '.switch_on_off', function(event, state){
    var data = $(this).data();
    
    param = {
        id: data.id,
        status: (state==true) ? "on" : "off"
    };

    $.getJSON('switch/change_switch_status', param)
        .done(function(result) {
            if (result.success == false) {
                alert_error(result.msg);
            }
        }).fail(function(jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
    });
});

$(".clear_logs").click(function() {
    var data = $(this).data();
    
    param = {
        id: 0,
        type: data.clear
    };
    
    $.getJSON('clear_logs.js', param)
        .done(function(result) {
            if (result.success) {
                update();
            } else {
                alert_error(result.msg);
            }
        }).fail(function(jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
    });
});

$(".stop_application").click(function() {
    $.getJSON('stop')
        .done(function(result) {
            if (result.success) {
                updateError = true;
                alert_info("Application stopped.");
            } else {
                alert_error(result.msg);
            }
        }).fail(function(jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
    });
});

$(".logout").click(function() {
    $.getJSON('auth/js_logout')
        .done(function(result) {
            if (result.success) {
                window.location.replace("/auth");
            } else {
                alert_error(result.msg);
            }
        }).fail(function(jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
    });
});

$('.container').on('click', '.change_heater_mode', function(){
    var data = $(this).data();
    var id = $(this).parentsUntil(".tab-pane").parent().attr('id').match(/\d+/g)[0];
    
    param = {
        heater: id,
        mode: data.mode
    };

    $.getJSON('thermostat/set_mode', param)
        .done(function(result) {
            if (result.success) {
                update();
            } else {
                alert_error(result.msg);
            }
        }).fail(function(jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
    });
});

$('.container').on('click', '.change_thermostat_status', function(){
    var data = $(this).data();

    $.getJSON('thermostat/set_status', data)
        .done(function(result) {
            if (result.success) {
                update();
            } else {
                alert_error(result.msg);
            }
        }).fail(function(jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
    });
});

$('.container').on('click', '.change_script_status', function(){
    var data = $(this).data();
	
    $('#script_status_'+data.id).find('span').first().text("...");

    $.getJSON('script/set_status', data)
        .done(function(result) {
            if (result.success) {
                update();
            } else {
                alert_error(result.msg);
            }
        }).fail(function(jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
    });
});

$('.container').on('click', '.change_heater_setpoint', function(){
    var data = $(this).data();
    var id = $(this).parentsUntil(".tab-pane").parent().attr('id').match(/\d+/g)[0];
    
    if (data.type === "heat") {
        temperature = parseFloat($('#heater_'+id).find(".heat_setpoint").text());
    } else {
        temperature = parseFloat($('#heater_'+id).find(".cool_setpoint").text());
    }
    
    if (isNaN(temperature)) {
        temperature = 0;
    }
    
    if (data.increment) {
        temperature += 0.5;
    } else {
        temperature -= 0.5;
    }
    
    param = {
        heater: id
    };
    
    if (data.type === "heat") {
        param.heat_temperature = temperature;
    } else {
        param.cool_temperature = temperature;
    }

    $.getJSON('thermostat/set_setpoint', param)
        .done(function(result) {
            if (result.success) {
                update();
            } else {
                alert_error(result.msg);
            }
        }).fail(function(jqxhr, textStatus, error) {
            alert_error("Request Failed: " + error);
    });
});

function alert_info(message) {           
    $('#alert_placeholder').prepend(
        '<div style="display: none;" class="alert alert-info fade in" role="alert">' +
            '<strong>Info!</strong> '+message+
            '<button type="button" class="close" data-dismiss="alert" aria-label="Close">' +
                '<span aria-hidden="true">&times;</span>' +
            '</button>'+
        '</div>'
    );
    
    $('.alert-info').slideDown( "fast" );
}

function alert_success(message) {           
    $('#alert_placeholder').prepend(
        '<div style="display: none;" class="alert alert-success fade in" role="alert">' +
            '<strong>Succes!</strong> '+message+
            '<button type="button" class="close" data-dismiss="alert" aria-label="Close">' +
                '<span aria-hidden="true">&times;</span>' +
            '</button>'+
        '</div>'
    );
    
    $('.alert-success').slideDown( "fast" );
}

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
