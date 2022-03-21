"use strict"

var editor = null;

$(document).ready(function(){

  // Initially close all tabs except the home tab
  $(".tab:not(#home-tab)").fadeOut(0);

  // Load initial status, and initial transform code
  update_status();
  load_transform_code();

  // On clicking a tab header, close the current tab and open the tab
  // corresponding to the tab header clicked
  $("input[name=tab-select] + label").click(function(){
    var target_element_id = "#" + this.htmlFor + "-tab";
    $(".tab:not(" + target_element_id + ")").fadeOut(0).last().queue(function(){
      $(target_element_id).fadeIn(0)
      $(this).dequeue();
    });
  });

  editor = ace.edit("transform-function");
  editor.setTheme("ace/theme/twilight");
  editor.session.setMode("ace/mode/c_cpp");

  // Trigger update indicators to hide initially
  $(".updating").fadeOut(0);

  // When an update button is clicked, trigger the update query
  $(".update-link").click(function(e){
    e.preventDefault();
    update_status();
  });

  // When the reload button is clicked, send an API call to the server
  $("#reload").click(function(e){
    e.preventDefault();
    load_transform_code();
  });
})


function load_transform_code() {
  var button = $("#reload");
  var old_state = button.html();
  button.html("<i class=\"fa fa-refresh fa-spin fa-fw\"></i>")

  $.ajax({
    url: "/casper_fetch_code",
  }).done(function(response){
    parse_transform_code_fetch(response);
  }).fail(function(error){
    parse_transform_code_fetch_error(error);
  }).always(function(){
    button.html(old_state);
  });
}

function parse_transform_code_fetch_error(error) {
  console.log("Error fetching transform code: ", error);
}

function parse_transform_code_fetch(response) {
  if (!"code" in response) {
    console.log("Error fetching transform code!");
    return;
  }

  if (null !== editor) editor.setValue(response.code);
}

function update_status() {
  $(".not-updating").fadeOut(0);
  $(".updating").fadeIn(0);

  var callback = function() {
    $(".updating").delay(300).fadeOut(0);
    $(".not-updating").delay(300).fadeIn(0);
  }

  $.ajax({
    url: "/casper_query",
  }).done(function(response){
    parse_update_status(response);
    callback();
  }).fail(function(error) {
    parse_update_error(error);
    callback();
  });
}

function parse_update_error(error) {
  console.log("Error parsing update status: ", error);
}

function parse_update_status(response) {
  var components = ["machine-state"];

  for (var i = 0; i < components.length; ++i) {
    if (components[i] in response) {
      $("#" + components[i]).html(response[components[i]]).removeClass("error");
    } else {
      $("#" + components[i]).html("Unknown <i class=\"fa fa-exclamation-triangle\" aria-hidden=\"true\"></i>").addClass("error");
      parse_update_status_error(components[i]);
    }
  }
}

function parse_update_status_error(missing_component) {
  console.log("Error parsing update status: expected component '" + missing_component + "' is missing.");
}
