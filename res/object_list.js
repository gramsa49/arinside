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

function initSchemaTable() {
    if (schemaList != null) { 
		schemaListObj = new FilterableTable('schemaList', 'formFilter', 'schemaListFilterResultCount');
		schemaListObj.typeFilter = new Array();
		schemaListObj.onHasTypeFilter(function() {
			var allTypeOn = true;
			var allTypeOff = true;
			for (i = 1; i < 6; i++) {
				this.typeFilter[i] = $('#multiFilter input[value="' + i + '"]').attr('checked');
				if (this.typeFilter[i]) { allTypeOff = false; }
				if (!this.typeFilter[i]) { allTypeOn = false; }
			}
			return !(allTypeOff || allTypeOn)			
		})
		.onCheckTypeFilterForRow(function(row) {
			return schemaListObj.typeFilter[row[5]];
		})
		.setNameIndex(1)
		.onCreateHtmlRow(createSchemaRowHtml);
	}
}

function updateSchemaTable() {
    if (schemaListObj != null) { schemaListObj.filterTable(); }
}

function createSchemaRowHtml(data) {
    return ($("<tr>")
        .append($("<td>")
            .append(getIcon(rootLevel, 1, 0))
            .append($("<a>").attr("href", data[8]).text(data[1]))
        )
        .append($("<td>").text(data[2]))
        .append($("<td>").text(data[3]))
		.append($("<td>").text(data[4]))
		.append($("<td>").text(ARSchemaType(data[5])))
		.append($("<td>").text(data[6]))
		.append($("<td>").text(data[7]))
    );
}

$('document').ready(function() {
    var checkBoxes = $('#multiFilter input[type="checkbox"]');

    $(".clearable").on('propertychange keyup input paste', 'input.data_field', function(e) {
        if (e.keyCode == 27 /*Escape-Key*/) { $(this).val(''); }
        $(this).stopTime().oneTime(300, updateSchemaTable);
    });
    $("#formFilter").click(updateSchemaTable);
    $("#typeFilterNone").click(function() {
        checkBoxes.each(function() {
            this.checked = false;
        });
        updateSchemaTable();
    });
    checkBoxes.change(updateSchemaTable);

    initSchemaTable();
    if ($("#formFilter").focus().val() != "" || schemaListObj.hasTypeFilter()) {
        updateSchemaTable();
    };

    $("#formLetterFilter a").click(function() {
        $("#formFilter").val("^" + this.text);
        updateSchemaTable();
        return false;
    });
});
