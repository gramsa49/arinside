function FilterableTable(objTableId, objFilterId, objResultCountId) {
	this.tableId = objTableId;
	this.filterId = objFilterId;
	this.resultCountId = objResultCountId;
	this.maxMatches = 100;
	this.lastMatches = 0;
	this.lastIndex = 0;
	this.nameIndex = 0;
    this.table = $('#' + objTableId);
}

FilterableTable.prototype.hasTypeFilter = function() { return false; }
FilterableTable.prototype.checkTypeFilterForRow = function(row) { return false; }
FilterableTable.prototype.createHtmlRow = function(row) { return ""; }

FilterableTable.prototype.onHasTypeFilter = function(cb) { this.hasTypeFilter = cb; return this; }
FilterableTable.prototype.onCheckTypeFilterForRow = function(cb) { this.checkTypeFilterForRow = cb; return this; }
FilterableTable.prototype.onCreateHtmlRow = function(cb) { this.createHtmlRow = cb; return this; }

FilterableTable.prototype.setNameIndex = function(idx) { this.nameIndex = idx; return this; }

FilterableTable.prototype.createMoreResultsRow = function() {
	var ft = this;
    return $("<tfoot>").append($("<tr>")
    .append($("<td class='warn' colspan=99>").text("Result limit reached! ")
	    .append($("<a id=showNext href='javascript:void(0)'>Show Next " + ft.maxMatches + "</a>").click(function() {
	        $(this).parents('tfoot:first').remove();
	        ft.filterTable('next');
	    }))
	    .append(" &nbsp; ")
	    .append($("<a id=showAll href='javascript:void(0)'>Show All</a>").click(function() {
	        $(this).parents('tfoot:first').remove();
	        ft.filterTable('all');
	    }))
	));
}

FilterableTable.prototype.filterTable = function(appendNextChunk) {
    var table = this.table; //$('#' + tableId);
	var list = window[this.tableId];
    var search = $('#' + this.filterId).val().replace(/ +/g, " ").replace(/ /g, ".*");
    var numSearch = search.search("^\\d+$");
    var maxMatch = this.maxMatches;
    var lastMatches = 0;
    var matches = 0;
    var hasTypeFilter = this.hasTypeFilter();
    var hasFilter = (search != null && search.length > 0) || hasTypeFilter;
    var start = 0;
    var end = list.length;
    var showAllMatches = appendNextChunk == 'all';

    if (appendNextChunk) {
        start = this.lastIndex;
        lastMatches += this.lastMatches;
    }
    else {
        $('#' + this.tableId + ' tbody,tfoot').remove();
    }

    if (hasFilter) {
        for (var i = start; i < end; i++) {
            var r = new RegExp(search, "i");
			var row = list[i];
            if ((!hasTypeFilter || hasTypeFilter && this.checkTypeFilterForRow(row)) && (row[this.nameIndex].match(r) || (numSearch == 0 && ("" + row[0]) == search))) {
                matches++;
                table.append(this.createHtmlRow(row));
            }
            if (!showAllMatches && matches >= maxMatch) {
                table.append(this.createMoreResultsRow());
                this.lastIndex = i + 1;
                this.lastMatches = lastMatches + matches;
                break;
            }
        };
    }
    $('#' + this.resultCountId).text((hasFilter ? "showing " + (lastMatches + matches) + " out of " : ""));
}

function initActlinkTable() {
    if (alList != null) { 
		alListObj = new FilterableTable('alList', 'actlinkFilter', 'actlinkListFilterResultCount'); 
		alListObj.onCreateHtmlRow(createActlinkRowHtml);
	}
}

function updateActlinkTable() {
    if (alListObj != null) { alListObj.filterTable(); }
}

function createActlinkRowHtml(data) {
    return ($("<tr>")
        .append($("<td>")
            .append(getIcon(rootLevel, 6))
            .append($("<a>").attr("href", data[9]).text(data[0]))
        )
        .append($("<td>").text(AREnabled(data[1])))
        .append($("<td>").text(data[2]))
		.append($("<td>").text(data[3]))
		.append($("<td>").text(data[4]))
		.append($("<td>").text(data[5]))
		.append($("<td>").text(data[6]))
		.append($("<td>").text(data[7]))
		.append($("<td>").text(data[8]))
    );
}

$('document').ready(function() {
    //var checkBoxes = $('#multiFilter input[type="checkbox"]');

    $(".clearable").on('propertychange keyup input paste', 'input.data_field', function(e) {
        if (e.keyCode == 27 /*Escape-Key*/) { $(this).val(''); }
        $(this).stopTime().oneTime(300, updateActlinkTable);
    });
    $("#execactlinkFilter").click(updateActlinkTable);
	/*
    $("#typeFilterAll").click(function() {
        $('#multiFilter input[type="checkbox"]').each(function() {
            this.checked = true;
        });
        $("#execFormFilter").click();
    });
    $("#typeFilterNone").click(function() {
        checkBoxes.each(function() {
            this.checked = false;
        });
        $("#execFormFilter").click();
    });
    $("#typeFilterInvert").click(function() {
        checkBoxes.each(function() {
            this.checked = !this.checked;
        });
        $("#execFormFilter").click();
    });
    checkBoxes.change(function() {
        $("#execFormFilter").click();
    });
	*/

    initActlinkTable();
    if ($("#actlinkFilter").focus().val() != "" || alListObj.hasTypeFilter()) {
        updateActlinkTable();
    };

    $("#formLetterFilter a").click(function() {
        $("#actlinkFilter").val("^" + this.text);
        updateActlinkTable();
        return false;
    });
});
