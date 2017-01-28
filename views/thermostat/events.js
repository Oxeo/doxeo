<script src="../assets/fullcalendar/moment.min.js" type="text/javascript"></script>
<script src="../assets/fullcalendar/fullcalendar.min.js" type="text/javascript"></script>
<script src="../assets/jquery-ui/jquery-ui.min.js" type="text/javascript"></script>

<script>
var CompCalendar = function() {
    var calendarEvents  = $('.calendar-events');

    /* Function for initializing drag and drop event functionality */
    var initEvents = function() {
        $('#external-events .fc-event').each(function() {
            // create an Event Object (http://arshaw.com/fullcalendar/docs/event_data/Event_Object/)
            var eventObject = { title: $.trim($(this).text()), color: $(this).css('background-color') };
            
            eventObject.heater_id = $(this).data("id");
            eventObject.occurrenceNumber = $(this).data("occurrence");

            // store the Event Object in the DOM element so we can get to it later
            $(this).data('eventObject', eventObject);

            // make the event draggable using jQuery UI
            $(this).draggable({ zIndex: 999, revert: true, revertDuration: 0 });
        });
    };

    return {
        init: function() {
            /* Initialize drag and drop event functionality */
            initEvents();

            /* Add new event in the events list */
            var eventInput      = $('#add-event');
            var eventInputVal   = '';

            // When the add button is clicked
            $('#add-event-btn').on('click', function(){
                // Get input value
                eventInputVal = eventInput.prop('value');

                // Check if the user entered something
                if ( eventInputVal ) {
                    // Add it to the events list
                    calendarEvents.append('<li class="animation-slideDown">' + $('<div />').text(eventInputVal).html() + '</li>');

                    // Clear input field
                    eventInput.prop('value', '');

                    // Init Events
                    initEvents();
                }

                // Don't let the form submit
                return false;
            });

            /* Initialize FullCalendar */
            var date = new Date();
            var d = date.getDate();
            var m = date.getMonth();
            var y = date.getFullYear();

            $('#calendar').fullCalendar({
                header: {
                    left: 'prev,next',
                    center: 'title',
                    right: 'month,agendaWeek,agendaDay'
                },
                displayEventEnd: true,
                defaultView: 'agendaWeek',
                height: 900,
                firstDay: 1,
                scrollTime: '10:00:00',
                timeFormat: 'H(:mm)',
                editable: true,
                droppable: true,
                drop: function(date, allDay) { // this function is called when something is dropped

                    // retrieve the dropped element's stored Event Object
                    var originalEventObject = $(this).data('eventObject');

                    // we need to copy it, so that multiple events don't have a reference to the same object
                    var copiedEventObject = $.extend({}, originalEventObject);
                    
                    var endDate = moment(date);
                    endDate.add(2, 'hours');

                    // assign it the date that was reported
                    copiedEventObject.start = date;
                    copiedEventObject.end = endDate;
                    
                    copiedEventObject.id = "new";
                    copiedEventObject.occurrenceId = 0;
                    
                    if (copiedEventObject.occurrenceNumber > 1) {
                        copiedEventObject.isRecurrent = true;
                    } else {
                        copiedEventObject.isRecurrent = false;
                    }

                    // render the event on the calendar
                    // the last `true` argument determines if the event "sticks" (http://arshaw.com/fullcalendar/docs/event_rendering/renderEvent/)
                    $('#calendar').fullCalendar('renderEvent', copiedEventObject, true);
                    
                    addEvent(copiedEventObject.heater_id, date, endDate, copiedEventObject.occurrenceNumber);
                },
                eventClick: function(event, jsEvent, view) {
                    $( "#FormEditEvent" ).data("event", event);
                    $( "#modalEventEditTitle" ).text(event.start.format('HH:mm') + " " + event.end.format('HH:mm'));
                    $( "#modalEventEdit" ).modal();
                },
                eventResize: function(event, delta, revertFunc) {
                    if (event.isRecurrent) {
                        updateEvent(event.id, event.start, event.end, true, revertFunc);
                    } else {
                        updateEvent(event.occurrenceId, event.start, event.end, false, revertFunc);
                    }
                },
                eventDrop: function(event, delta, revertFunc) {
                    if (event.isRecurrent) {
                        updateEvent(event.id, event.start, event.end, true, revertFunc);
                    } else {
                        updateEvent(event.occurrenceId, event.start, event.end, false, revertFunc);
                    }
                },
                events: function(start, end, timezone, callback) {                  
                    var param = {
                        start: start.format('YYYY-MM-DD'),
                        end: end.format('YYYY-MM-DD')
                    };
                    
                    $.getJSON('events.js', param).done(function(result) {
                        if (result.success) {
                            var events = [];
                            $.each(result.records, function(key, event) {
                                var object = {};
                                
                                if (event.recurrente_date) {
                                    object.id = event.event_id;
                                    object.isRecurrent = true;
                                } else {
                                    object.isRecurrent = false;
                                }
                                
                                object.occurrenceId = event.occurrence_id;
                                object.title = ((event.recurrente_date)?"(R) ":"") + event.heater_id;
                                object.start = new Date(event.start_date);
                                object.end = new Date(event.end_date);
                                object.allDay = false;
                                object.color = (event.heater_id==1)?"#1abc9c":"#9b59b6";
                                
                                events.push(object);
                            });
                            callback(events);
                        } else {
                            alert_error(result.msg);
                        }
                    }).fail(function(jqxhr, textStatus, error) {
                        alert_error("Request Failed: " + error);
                    });
                    
                }
            });
        }
    };
}();

$(function(){ CompCalendar.init(); });

$('#FormEditEvent').on('submit', function(e) {
    e.preventDefault();

    var event = $( "#FormEditEvent" ).data("event");
    var remove = $('input[name=radioRemove]:checked', '#FormEditEvent').val();

    if (remove === "link" && event.isRecurrent && event.occurrenceId != 0) {
        // Update time to break the reccurrent status
        updateEvent(event.occurrenceId, event.start, event.end, false, null);

        event._id = "oc" + event.occurrenceId;
        event.isRecurrent = false;
        event.title = event.title.substring(4);
        $('#calendar').fullCalendar('updateEvent', event);
        $( "#modalEventEdit" ).modal('hide');
    }

    if (remove === "event" && event.occurrenceId != 0) {
        deleteEvent(event.occurrenceId, false);

        if (event.isRecurrent) {
            event._id = "oc" + event.occurrenceId;
            $('#calendar').fullCalendar('updateEvent', event);
        }

        $('#calendar').fullCalendar('removeEvents', event._id);
        $( "#modalEventEdit" ).modal('hide');
    }

    if (remove === "all_occurrences" && event.isRecurrent) {
        deleteEvent(event.id, true)
        $('#calendar').fullCalendar('removeEvents', event._id);
        $( "#modalEventEdit" ).modal('hide');
    }
});

function addEvent(heaterId, startDate, endDate, occurrence_number) {
    var param = {
        heater_id: heaterId,
        start_date: startDate.format('YYYY-MM-DD%20HH:mm:ss'),
        end_date: endDate.format('YYYY-MM-DD%20HH:mm:ss'),
        setpoint: 'heat',
        occurrence_number: occurrence_number,
    };

    $.getJSON('add_event', param)
        .done(function(result) {
            if (result.success) {
                var event = $('#calendar').fullCalendar('clientEvents', "new")[0];
                
                if (event.isRecurrent) {
                    event.id = result.event_id;
                    event.occurrenceId = 0;
                } else {
                    event.id = "n" + result.occurrence_id;
                    event.occurrenceId = result.occurrence_id;
                }
                
                event._id = event.id;
                
                $('#calendar').fullCalendar('updateEvent', event);
                
                $.bootstrapGrowl('<h4>Success!</h4> <p>' + startDate.format('HH:mm') + ' to ' + endDate.format('HH:mm') + '</p>', {
                    type: "success",
                    delay: 2500,
                    allow_dismiss: true
                });
            } else {
                $.bootstrapGrowl('<h4>Error!</h4> <p>' + result.msg  + '</p>', {
                    type: "danger",
                    delay: 0,
                    allow_dismiss: true
                });
            }
        }).fail(function(jqxhr, textStatus, error) {
            $.bootstrapGrowl('<h4>Error!</h4> <p>Request Failed: ' + error  + '</p>', {
                type: "danger",
                delay: 0,
                allow_dismiss: true
            });
    });
};

function updateEvent(id, startTime, endTime, allOccurrence, revertFunc) {
    var param = {
        id: id,
        start: startTime.format('HH:mm:ss'),
        end: endTime.format('HH:mm:ss'),
        all_occurrences: allOccurrence,
    };

    $.getJSON('set_event_time', param)
        .done(function(result) {
            if (result.success) {
                $.bootstrapGrowl('<h4>Success!</h4> <p>' + startTime.format('HH:mm') + ' to ' + endTime.format('HH:mm') + '</p>', {
                    type: "success",
                    delay: 2500,
                    allow_dismiss: true
                });
            } else {
                $.bootstrapGrowl('<h4>Error!</h4> <p>' + result.msg  + '</p>', {
                    type: "danger",
                    delay: 0,
                    allow_dismiss: true
                });
                revertFunc();
            }
        }).fail(function(jqxhr, textStatus, error) {
            $.bootstrapGrowl('<h4>Error!</h4> <p>Request Failed: ' + error  + '</p>', {
                type: "danger",
                delay: 0,
                allow_dismiss: true
            });
            revertFunc();
    });
};

function deleteEvent(id, allOccurrence) {
    var param = {};
    
    if (allOccurrence) {
        param.event_id = id;
    } else {
        param.occurrence_id = id;
    }

    $.getJSON('delete_event', param)
        .done(function(result) {
            if (result.success) {
                $.bootstrapGrowl('<h4>Success!</h4> <p>Deleted with success</p>', {
                    type: "success",
                    delay: 2500,
                    allow_dismiss: true
                });
            } else {
                $.bootstrapGrowl('<h4>Error!</h4> <p>' + result.msg  + '</p>', {
                    type: "danger",
                    delay: 0,
                    allow_dismiss: true
                });
                revertFunc();
            }
        }).fail(function(jqxhr, textStatus, error) {
            $.bootstrapGrowl('<h4>Error!</h4> <p>Request Failed: ' + error  + '</p>', {
                type: "danger",
                delay: 0,
                allow_dismiss: true
            });
            revertFunc();
    });
};

</script>