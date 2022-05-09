
# Probleme mit Standard

- Folgende Typen sind nicht in ValueTypes repräsentiert
  - string_array
  - integer_matrix
  - boolean_matrix
  - string_matrix

# Probleme mit Dateien

## FVA_worm_stage_1-4.rexs

- material_type_din_743_2012 is ein enum, aber in der Datei ein Integer (2 und 3)

## rexs_model_1.4_en.xml

- enum material_type_din_743_2012 hat einen leeren Wert (<enumValue value="" name="TODO"/>)
