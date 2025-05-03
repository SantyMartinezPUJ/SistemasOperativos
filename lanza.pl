#!/usr/bin/perl
#**************************************************************
#         		Pontificia Universidad Javeriana
#     Autor: J. Corredor
#     Fecha: Febrero 2024
#     Materia: Sistemas Operativos
#     Tema: Taller de Evaluación de Rendimiento
#     Fichero: script automatización ejecución por lotes 
#****************************************************************/

$Path = `pwd`;
chomp($Path);

$Nombre_Ejecutable = "MM_ejecutable";
@Size_Matriz = ("200","400","800");
@Num_Hilos = (1,2,4,8);
$Repeticiones = 30;

#foreach $size (@Size_Matriz){
#	foreach $hilo (@Num_Hilos) {
#		$file = "$Path/$Nombre_Ejecutable-".$size."-Hilos-".$hilo.".csv";
#		for ($i=0; $i<$Repeticiones; $i++) {
#system("$Path/$Nombre_Ejecutable $size $hilo  >> $file");
#			printf("$Path/$Nombre_Ejecutable $size $hilo \n");
#		}
#		close($file);
#	$p=$p+1;
#	}
#}

foreach $size (@Size_Matriz) {
	foreach $hilo (@Num_Hilos) {
		$file = "$Path/$Nombre_Ejecutable-${size}-Hilos-${hilo}.csv";
		
		# Escribimos encabezado CSV
		open(FH, ">$file") or die "No se pudo abrir archivo: $file";
		print FH "Repeticion,Tiempo_us\n";
		close(FH);

		for ($i = 1; $i <= $Repeticiones; $i++) {
			# Ejecutamos y capturamos el tiempo
			$tiempo = `$Path/$Nombre_Ejecutable $size $hilo`;
			chomp($tiempo);

			# Guardamos en el CSV
			open(FH, ">>$file") or die "No se pudo escribir en archivo: $file";
			print FH "$i,$tiempo\n";
			close(FH);

			print "Ejecutado: $Nombre_Ejecutable $size $hilo [Repeticion $i]\n";
		}
	}
}
